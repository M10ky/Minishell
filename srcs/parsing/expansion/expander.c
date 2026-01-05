/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 07:44:36 by tarandri          #+#    #+#             */
/*   Updated: 2026/01/05 21:59:32 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/parsing.h"

static char	*handle_dollar_expansion(char *str, int *i, char *result,
				t_shell *shell)
{
	char	*var_name;
	char	*var_value;

	(*i)++;
	if (str[*i] == '?')
	{
		var_value = ft_itoa(shell->last_exit_status);
		result = str_append_str(result, var_value);  // str_append_str libère result
		free(var_value);
		(*i)++;
		return (result);
	}
	if (str[*i] == '{')
		var_name = extract_var_name_braces(str, i);
	else if (is_valid_var_char(str[*i]))
		var_name = extract_var_name_simple(str, i);
	else
	{
		result = str_append_char(result, '$');  // str_append_char libère result
		return (result);
	}
	if (var_name)
	{
		var_value = get_env_value_copy(shell, var_name);
		if (var_value)
		{
			result = str_append_str(result, var_value);  // str_append_str libère result
			free(var_value);
		}
		free(var_name);
	}
	return (result);
}
static int	is_heredoc_delimiter(t_token *tokens, t_token *current)
{
	t_token	*prev;

	if (!tokens || !current)
		return (0);

	prev = tokens;

	// Cas spécial: current est le premier token
	if (prev == current)
		return (0);

	// Chercher le token précédent
	while (prev && prev->next != current)
		prev = prev->next;

	// Si le token précédent est <<, alors current est un délimiteur
	if (prev && prev->type == HEREDOC)
		return (1);

	return (0);
}
void	expand_tokens(t_token *tokens, t_shell *shell)
{
	t_token	*current;
	char	*expanded;

	current = tokens;
	while (current)
	{
		// NE PAS expanser les délimiteurs de heredoc
		if (current->type == WORD && is_heredoc_delimiter(tokens, current))
		{
			// Le délimiteur reste tel quel (même avec des $)
			current = current->next;
			continue;
		}

		// Expanser les autres WORD normalement
		if (current->type == WORD)
		{
			expanded = expand_string(current->value, shell);
			free(current->value);
			current->value = expanded;
		}
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
			result = handle_dollar_expansion(str, &i, result, shell);
		else
			result = str_append_char(result, str[i++]);
	}
	return (result);
}
