/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 07:44:36 by tarandri          #+#    #+#             */
/*   Updated: 2026/01/08 16:48:16 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/parsing.h"

static char	*handle_dollar_expansion(char *str, int *i, char *result,
				t_shell *shell, int in_dq)
{
	char	*var_name;
	char	*var_value;

	(*i)++;  // Passer le '$'

	// Cas 1: $? - Code de sortie
	if (str[*i] == '?')
	{
		var_value = ft_itoa(shell->last_exit_status);
		result = str_append_str(result, var_value);
		free(var_value);
		(*i)++;
		return (result);
	}
	if ((str[*i] == '"' || str[*i]=='\'') && !in_dq)
		return (result);
	// Cas 2: ${VAR} - Variable avec accolades
	if (str[*i] == '{')
	{
		var_name = extract_var_name_braces(str, i);
		if (!var_name)
		{
			// Erreur de syntaxe dans ${...} - garder littéral
			result = str_append_str(result, "${");
			return (result);
		}
		var_value = get_env_value_copy(shell, var_name);
		if (var_value)
		{
			result = str_append_str(result, var_value);
			free(var_value);
		}
		// Si la variable n'existe pas, on ajoute rien (expansion vide)
		free(var_name);
		return (result);
	}

	// Cas 3: $VAR - Variable simple
	if (is_valid_var_char(str[*i]))
	{
		var_name = extract_var_name_simple(str, i);
		if (!var_name)
		{
			// Cas bizarre, garder le $
			result = str_append_char(result, '$');
			return (result);
		}
		var_value = get_env_value_copy(shell, var_name);
		if (var_value)
		{
			result = str_append_str(result, var_value);
			free(var_value);
		}
		// Si la variable n'existe pas, on ajoute rien (expansion vide)
		free(var_name);
		return (result);
	}

	// Cas 4: $ suivi d'un caractère invalide ou rien
	// → Garder le $ littéral
	result = str_append_char(result, '$');
	return (result);
}

void	expand_tokens(t_token *tokens, t_shell *shell)
{
	t_token	*current;
	t_token	*prev;
	char	*expanded;
	int		original_was_quoted;  // NOUVEAU

	current = tokens;
	prev = NULL;
	while (current)
	{
		if (current->type == WORD && (!prev || prev->type != HEREDOC))
		{
			original_was_quoted = current->was_quoted;  // Sauvegarder
			// DEBUG: AJOUTER CES LIGNES
			// printf("🔍 DEBUG expand AVANT: value='%s', was_quoted=%d\n",
			    //    current->value, current->was_quoted);
			expanded = expand_string(current->value, shell);
			free(current->value);
			current->value = expanded;
			current->was_quoted = original_was_quoted;  // RESTAURER
			// DEBUG: AJOUTER CES LIGNES
			// printf("🔍 DEBUG expand APRES: value='%s', was_quoted=%d\n",
			    //    current->value, current->was_quoted);
		}
		prev = current;
		current = current->next;
	}
}

char	*expand_string(char *str, t_shell *shell)
{
	char	*result;
	int		i;
	int		in_single_quote;
	int		in_double_quote;

	result = ft_strdup("");
	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;

	while (str[i])
	{
		if (str[i] == '\'' && !in_double_quote)
		{
			in_single_quote = !in_single_quote;
			i++;
		}
		else if (str[i] == '"' && !in_single_quote)
		{
			in_double_quote = !in_double_quote;
			i++;
		}
		else if (str[i] == '$' && !in_single_quote)
			result = handle_dollar_expansion(str, &i, result, shell, in_double_quote);
		else
			result = str_append_char(result, str[i++]);
	}
	return (result);
}


