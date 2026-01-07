/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 10:49:05 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/07 20:45:05 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/exec.h"

t_env *new_env_node(char *str)
{
	t_env	*node;
	int		i;
	char	*key;
	char	*value;

	if (!str)
		return (NULL);
	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);

	// Trouve le '='
	i = 0;
	while (str[i] && str[i] != '=')
		i++;

	// Extraire la clé
	key = ft_substr(str, 0, i);
	if (!key)
	{
		free(node);
		return (NULL);
	}

	// Extraire la valeur (après '=')
	if (str[i] == '=')
		value = ft_strdup(str + i + 1);
	else
		value = ft_strdup("");

	if (!value)
	{
		free(key);
		free(node);
		return (NULL);
	}

	node->key = key;
	node->value = value;
	node->next = NULL;
	return (node);
}

/* NOUVEAU: Créer un nœud d'environnement à partir d'une clé et valeur séparées */
static t_env *new_env_node_kv(char *key, char *value)
{
	t_env	*node;

	if (!key || !value)
		return (NULL);
	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);

	node->key = ft_strdup(key);
	node->value = ft_strdup(value);

	if (!node->key || !node->value)
	{
		if (node->key)
			free(node->key);
		if (node->value)
			free(node->value);
		free(node);
		return (NULL);
	}

	node->next = NULL;
	return (node);
}

/* NOUVEAU: Ajouter un nœud à la fin de la liste */
static void add_env_node(t_env **env, t_env *node)
{
	t_env *curr;

	if (!env || !node)
		return;

	if (!*env)
	{
		*env = node;
		return;
	}

	curr = *env;
	while (curr->next)
		curr = curr->next;
	curr->next = node;
}

/* NOUVEAU: Initialiser PWD si absent */
static void init_pwd(t_env **env)
{
	char	*cwd;
	t_env	*node;

	// Vérifier si PWD existe déjà
	if (get_env_value(*env, "PWD"))
		return;

	// Obtenir le répertoire courant
	cwd = getcwd(NULL, 0);
	if (!cwd)
		cwd = ft_strdup("/");  // Fallback

	node = new_env_node_kv("PWD", cwd);
	free(cwd);

	if (node)
		add_env_node(env, node);
}

/* NOUVEAU: Initialiser SHLVL */
static void init_shlvl(t_env **env)
{
	char	*shlvl_str;
	int		shlvl;
	char	*new_shlvl;
	t_env	*node;
	t_env	*curr;

	// Chercher SHLVL existant
	shlvl_str = get_env_value(*env, "SHLVL");

	if (shlvl_str)
	{
		// Incrémenter SHLVL existant
		shlvl = ft_atoi(shlvl_str);
		if (shlvl < 0)
			shlvl = 0;
		shlvl++;

		// Mettre à jour la valeur existante
		new_shlvl = ft_itoa(shlvl);
		curr = *env;
		while (curr)
		{
			if (ft_strcmp(curr->key, "SHLVL") == 0)
			{
				free(curr->value);
				curr->value = new_shlvl;
				return;
			}
			curr = curr->next;
		}
		free(new_shlvl);
	}
	else
	{
		// Créer SHLVL=1
		node = new_env_node_kv("SHLVL", "1");
		if (node)
			add_env_node(env, node);
	}
}

/* NOUVEAU: Initialiser _ (underscore) */
static void init_underscore(t_env **env)
{
	t_env	*node;

	// Vérifier si _ existe déjà
	if (get_env_value(*env, "_"))
		return;

	// Créer _=/usr/bin/env (valeur par défaut)
	node = new_env_node_kv("_", "/usr/bin/env");
	if (node)
		add_env_node(env, node);
}

/* MODIFIÉ: Fonction principale d'initialisation de l'environnement */
void init_env(t_shell *shell, char **envp)
{
	t_env	*curr;
	t_env	*new;
	int		i;

	if (!shell)
		return;

	shell->env = NULL;

	// 1. Dupliquer l'environnement fourni (peut être vide)
	i = 0;
	while (envp && envp[i])
	{
		new = new_env_node(envp[i]);
		if (!new)
		{
			free_env(shell->env);
			shell->env = NULL;
			ft_putstr_fd("minishell: malloc error\n", 2);
			exit(1);
		}

		if (!shell->env)
			shell->env = new;
		else
		{
			curr = shell->env;
			while (curr->next)
				curr = curr->next;
			curr->next = new;
		}
		i++;
	}

	// 2. Initialiser les variables essentielles (comme bash)
	init_pwd(&shell->env);      // PWD
	init_shlvl(&shell->env);    // SHLVL
	init_underscore(&shell->env); // _
}

char *get_env_value(t_env *env, char *key)
{
	if (!env || !key)
		return (NULL);
	while (env)
	{
		if (ft_strcmp(env->key, key) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

int count_env_vars(t_env *env)
{
	int count = 0;

	while (env)
	{
		count++;
		env = env->next;
	}
	return (count);
}

char **env_to_tab(t_env *env)
{
	char	**env_tab;
	char	*temp;
	int		count;
	int		i;

	if (!env)
		return (NULL);
	count = count_env_vars(env);
	env_tab = (char **)malloc(sizeof(char *) * (count + 1));
	if (!env_tab)
		return (NULL);

	i = 0;
	while (env)
	{
		temp = ft_strjoin(env->key, "=");
		if (!temp)
		{
			free_tab_partial(env_tab, i);
			return (NULL);
		}
		env_tab[i] = ft_strjoin(temp, env->value);
		free(temp);
		if (!env_tab[i])
		{
			free_tab_partial(env_tab, i);
			return (NULL);
		}
		env = env->next;
		i++;
	}
	env_tab[i] = NULL;
	return (env_tab);
}

void	exp_add_env_node_back(t_env **head, t_env *new_node)
{
	t_env	*current;

	if (!head || !new_node)
		return ;
	if (*head == NULL)
	{
		*head = new_node;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_node;
}

void update_env_var(t_env *env, char *key, char *new_value)
{
	t_env *current;

	if (!env || !key || !new_value)
		return;
	current = env;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			free(current->value);
			current->value = ft_strdup(new_value);
			return;
		}
		current = current->next;
	}
}
