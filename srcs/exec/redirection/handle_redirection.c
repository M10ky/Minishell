/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:32:52 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/03 16:09:22 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/exec.h"

/**
 * Gère les redirections d'entrée
 *
 * CORRECTION:
 * - Vérification supplémentaire de last_redir->file
 * - Gestion d'erreur améliorée
 *
 * EXPLICATION:
 * En cas de redirections multiples (< f1 < f2 < f3):
 * - Seul f3 (le dernier) est utilisé pour STDIN
 * - Les autres fichiers ne sont même pas ouverts
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
	{
		ft_putstr_fd("minishell: ambiguous redirect\n", 2);
		return (1);
	}

	// Ouvrir le fichier en lecture
	fd = open(last_redir->file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(last_redir->file, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd("\n", 2);
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
 *
 * CORRECTION IMPORTANTE:
 * - Vérification de chaque open()
 * - Fermeture systématique des FD
 * - Gestion d'erreur à chaque étape
 *
 * EXPLICATION DU COMPORTEMENT:
 * Pour: echo test > f1 > f2 > f3
 * 1. Ouvrir f1 (crée/tronque), fermer immédiatement
 * 2. Ouvrir f2 (crée/tronque), fermer immédiatement
 * 3. Ouvrir f3 (crée/tronque), le garder pour STDOUT
 *
 * POURQUOI?
 * - Bash crée TOUS les fichiers (même s'ils sont vides)
 * - Seul le dernier reçoit vraiment la sortie
 */
static int	handle_output_redirections(t_redir *output_list)
{
	t_redir	*current;
	t_redir	*last_redir;
	int		fd;

	if (!output_list)
		return (0);

	// ÉTAPE 1: Créer/tronquer tous les fichiers sauf le dernier
	current = output_list;
	while (current->next)
	{
		// Vérifier que le fichier n'est pas NULL
		if (!current->file)
		{
			ft_putstr_fd("minishell: ambiguous redirect\n", 2);
			return (1);
		}

		// Ouvrir et fermer immédiatement (crée/tronque le fichier)
		fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(current->file, 2);
			ft_putstr_fd(": ", 2);
			ft_putstr_fd(strerror(errno), 2);
			ft_putstr_fd("\n", 2);
			return (1);
		}
		close(fd);  // IMPORTANT: Fermer immédiatement
		current = current->next;
	}

	// ÉTAPE 2: Utiliser le dernier fichier pour STDOUT
	last_redir = current;
	if (!last_redir->file)
	{
		ft_putstr_fd("minishell: ambiguous redirect\n", 2);
		return (1);
	}

	fd = open(last_redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(last_redir->file, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd("\n", 2);
		return (1);
	}

	// Rediriger STDOUT vers le fichier
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		return (1);
	}

	close(fd);  // Fermer après dup2
	return (0);
}

/**
 * Gère les redirections en mode append (>>)
 *
 * CORRECTION:
 * - Même logique que handle_output_redirections
 * - Mais avec O_APPEND au lieu de O_TRUNC
 *
 * EXPLICATION:
 * Pour: echo test >> f1 >> f2 >> f3
 * - f1 et f2 sont ouverts en mode append puis fermés
 * - Seul f3 reçoit vraiment la sortie en mode append
 */
static int	handle_append_redirections(t_redir *append_list)
{
	t_redir	*current;
	t_redir	*last_redir;
	int		fd;

	if (!append_list)
		return (0);

	// ÉTAPE 1: Ouvrir/fermer tous les fichiers sauf le dernier
	current = append_list;
	while (current->next)
	{
		if (!current->file)
		{
			ft_putstr_fd("minishell: ambiguous redirect\n", 2);
			return (1);
		}

		fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd == -1)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(current->file, 2);
			ft_putstr_fd(": ", 2);
			ft_putstr_fd(strerror(errno), 2);
			ft_putstr_fd("\n", 2);
			return (1);
		}
		close(fd);
		current = current->next;
	}

	// ÉTAPE 2: Utiliser le dernier fichier
	last_redir = current;
	if (!last_redir->file)
	{
		ft_putstr_fd("minishell: ambiguous redirect\n", 2);
		return (1);
	}

	fd = open(last_redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(last_redir->file, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd("\n", 2);
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
 *
 * EXPLICATION DE L'ORDRE:
 * 1. Input AVANT output (stdin puis stdout)
 * 2. Heredoc prioritaire sur < (plus spécifique)
 * 3. Output puis append (ordre d'apparition)
 */
int	handle_redirections(t_command *cmd)
{
	if (!cmd)
		return (0);

	// 1. Gérer les redirections d'entrée
	// Heredoc a priorité sur les redirections simples
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
