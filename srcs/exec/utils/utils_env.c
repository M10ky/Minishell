/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 10:49:05 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/02 10:52:11 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/exec.h"


t_env *new_env_node(char *str)
{
    t_env *node;
    int i = 0;

    node = malloc(sizeof(t_env));
    if (!node)
    return NULL;

    // Trouve le '='
    while (str[i] && str[i] != '=')
        i++;

    node->key = strndup(str, i); // Utilise ton ft_strndup
    node->value = ft_strdup(str + i + 1); // Utilise ton ft_strdup
    node->next = NULL;
    return (node);
}

void init_env(t_shell *shell, char **envp)
{
    t_env *curr;
    t_env *new;
    int i = 0;

    shell->env = NULL;
    while (envp[i])
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
}
char *get_env_value(t_env *env, char *key)
{
    while (env)
    {
        // On compare la clé (ex: "PATH") avec env->key
        if (strncmp(env->key, key, strlen(key)) == 0
            && strlen(env->key) == strlen(key))
        {
            return (env->value);
        }
        env = env->next;
    }
    return (NULL); // Pas trouvé
}
/* utils_env.c */

// Compte le nombre de variables dans la liste chaînée
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

// Convertit la liste t_env en un tableau de char* pour execve
char **env_to_tab(t_env *env)
{
    char    **env_tab;
    char    *temp;
    int     count;
    int     i;

    count = count_env_vars(env);
    // Allouer le tableau (taille + 1 pour le NULL terminateur)
    env_tab = (char **)malloc(sizeof(char *) * (count + 1));
    if (!env_tab)
        return (NULL);

    i = 0;
    while (env)
    {
        // 1. Concaténer 'key' et '='
        temp = ft_strjoin(env->key, "=");
        if (!temp)
        {
            free_tab_partial(env_tab, i);
            return (NULL);
        }

        // 2. Concaténer 'key=' et 'value'
        env_tab[i] = ft_strjoin(temp, env->value);
        free(temp); // Libérer la chaîne intermédiaire

        if (!env_tab[i])
        {
            free_tab_partial(env_tab, i);
            return (NULL);
        }

        env = env->next;
        i++;
    }
    // 3. Terminer le tableau par NULL (Requis par execve)
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
    t_env *current = env;

    // Chercher la variable
    while (current)
    {
        if (ft_strcmp(current->key, key) == 0)
        {
            // Trouvée : remplacer la valeur
            free(current->value);
            current->value = ft_strdup(new_value);
            return;
        }
        current = current->next;
    }
    // add_env_node_back(&env, new_env_node(
    //     ft_strjoin(key, ft_strjoin("=", new_value))));
    // Pas trouvée : créer un nouveau nœud (optionnel selon le sujet)
    // Pour PWD/OLDPWD, elles existent normalement toujours
}
