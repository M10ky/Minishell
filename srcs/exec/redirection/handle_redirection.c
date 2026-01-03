/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:32:52 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/02 16:46:11 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/exec.h"

/**
 * Gère les redirections d'entrée
 * En cas de redirections multiples, seule la DERNIÈRE est utilisée (comportement bash)
 * Exemple: cat < file1 < file2 < file3  → utilise file3
 */
static int	handle_input_redirections(t_redir *input_list)
{
	t_redir	*last_redir;
	int		fd;

	if (!input_list)
		return (0);

	// Récupérer la dernière redirection
	last_redir = get_last_redir(input_list);
	if (!last_redir || !last_redir->file)
		return (0);

	// Ouvrir le fichier en lecture
	fd = open(last_redir->file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(last_redir->file);
		return (1);
	}

	// Rediriger STDIN vers le fichier
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		return (1);
	}

	close(fd);
	return (0);
}

/**
 * Gère les redirections de sortie (>)
 * En cas de redirections multiples, seule la DERNIÈRE est utilisée
 * MAIS tous les fichiers sont créés/tronqués
 * Exemple: echo test > file1 > file2 > file3  → écrit dans file3, mais file1 et file2 sont créés vides
 */
static int	handle_output_redirections(t_redir *output_list)
{
	t_redir	*current;
	t_redir	*last_redir;
	int		fd;

	if (!output_list)
		return (0);

	// ÉTAPE 1: Ouvrir et fermer tous les fichiers sauf le dernier (pour les créer/tronquer)
	current = output_list;
	while (current->next)
	{
		fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			ft_putstr_fd("minishell: ", 2);
			perror(current->file);
			return (1);
		}
		close(fd);
		current = current->next;
	}

	// ÉTAPE 2: Utiliser le dernier fichier pour la sortie réelle
	last_redir = current;  // current est maintenant le dernier nœud
	fd = open(last_redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(last_redir->file);
		return (1);
	}

	// Rediriger STDOUT vers le fichier
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		return (1);
	}

	close(fd);
	return (0);
}

/**
 * Gère les redirections en mode append (>>)
 * Même principe que les redirections de sortie
 */
static int	handle_append_redirections(t_redir *append_list)
{
	t_redir	*current;
	t_redir	*last_redir;
	int		fd;

	if (!append_list)
		return (0);

	// ÉTAPE 1: Ouvrir et fermer tous les fichiers sauf le dernier
	current = append_list;
	while (current->next)
	{
		fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd == -1)
		{
			ft_putstr_fd("minishell: ", 2);
			perror(current->file);
			return (1);
		}
		close(fd);
		current = current->next;
	}

	// ÉTAPE 2: Utiliser le dernier fichier
	last_redir = current;
	fd = open(last_redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(last_redir->file);
		return (1);
	}

	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		return (1);
	}

	close(fd);
	return (0);
}

/**
 * Fonction principale de gestion des redirections
 * Gère l'ordre correct: input → output/append
 */
int	handle_redirections(t_command *cmd)
{
	if (!cmd)
		return (0);

	// 1. Gérer les redirections d'entrée (< ou heredoc)
	//    Le heredoc a priorité sur les redirections simples
	if (cmd->heredoc)
	{
		if (handle_input_redirections(cmd->heredoc))
			return (1);
	}
	else if (cmd->input_redirection)
	{
		if (handle_input_redirections(cmd->input_redirection))
			return (1);
	}

	// 2. Gérer les redirections de sortie
	//    L'append a priorité sur les redirections simples si présent en dernier
	//    Mais en réalité, bash respecte l'ordre d'apparition
	//    Pour simplifier, on traite d'abord > puis >>
	if (cmd->output_redirection)
	{
		if (handle_output_redirections(cmd->output_redirection))
			return (1);
	}

	if (cmd->append)
	{
		if (handle_append_redirections(cmd->append))
			return (1);
	}

	return (0);
}
