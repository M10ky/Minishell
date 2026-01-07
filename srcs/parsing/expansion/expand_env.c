/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tarandri <tarandri@student.42antananarivo. +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 10:39:18 by tarandri          #+#    #+#             */
/*   Updated: 2026/01/07 06:26:30 by tarandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/parsing.h"

char	*get_env_value_copy(t_shell *shell, char *name)
{
	t_env	*current;

	if (!name || !shell || !shell->env)
		return (NULL);
	current = shell->env;
	while (current)
	{
		if (ft_strcmp(current->key, name) == 0)
		{
			if (current->value)
				return (ft_strdup(current->value));
			else
				return (ft_strdup(""));
		}
		current = current->next;
	}
	return (NULL);
}

char	*extract_var_name_braces(char *str, int *i)
{
	int	start;
	int	len;

	(*i)++;  // Passer le '{'
	start = *i;
	len = 0;
	
	// Vérifier que le premier caractère est valide
	if (!str[*i] || str[*i] == '}')
	{
		// ${} est invalide
		return (NULL);
	}
	
	// Le premier caractère doit être une lettre ou _
	if (!ft_isalpha(str[*i]) && str[*i] != '_')
	{
		// Caractère invalide en début de nom de variable
		return (NULL);
	}
	
	while (str[*i] && str[*i] != '}')
	{
		if (!is_valid_var_char(str[*i]))
		{
			// Caractère invalide dans le nom
			return (NULL);
		}
		len++;
		(*i)++;
	}
	
	if (str[*i] != '}')
	{
		// Accolade fermante manquante
		return (NULL);
	}
	
	(*i)++;  // Passer le '}'
	return (ft_substr(str, start, len));
}

char	*extract_var_name_simple(char *str, int *i)
{
	int	start;
	int	len;

	start = *i;
	len = 0;
	
	// Le premier caractère doit être une lettre ou _
	if (!ft_isalpha(str[*i]) && str[*i] != '_')
		return (NULL);
	
	// Ensuite on accepte lettres, chiffres et _
	while (str[*i] && is_valid_var_char(str[*i]))
	{
		len++;
		(*i)++;
	}
	
	if (len == 0)
		return (NULL);
		
	return (ft_substr(str, start, len));
}

int	is_valid_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}
