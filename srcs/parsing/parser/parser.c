/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 07:45:47 by tarandri          #+#    #+#             */
/*   Updated: 2026/01/02 11:44:54 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/parsing.h"

static t_command	*init_command(void)
{
	t_command	*command;

	command = malloc(sizeof(t_command));
	if (!command)
		return (NULL);
	command->args = malloc(sizeof(char *) * 1);  // FIX: Initialiser args
	if (!command->args)
	{
		free(command);
		return (NULL);
	}
	command->args[0] = NULL;  // FIX: Terminer par NULL
	command->input_redirection = NULL;
	command->output_redirection = NULL;
	command->is_heredoc = 0;
	command->append_output = 0;
	command->value_return = 0;
	command->next = NULL;
	return (command);
}

int	finalize_command(t_command **command_list, t_command **current_cmd)
{
	t_command	*temp;

	if (!*current_cmd)
		return (0);
	if (!*command_list)
		*command_list = *current_cmd;
	else
	{
		temp = *command_list;
		while (temp->next)
			temp = temp->next;
		temp->next = *current_cmd;
	}
	*current_cmd = NULL;
	return (1);
}

t_command	*parse(t_token *tokens)
{
	t_command	*command_list;
	t_command	*current_cmd;
	t_token		*current;

	command_list = NULL;
	current_cmd = init_command();
	current = tokens;
	if (!tokens || !current_cmd)
		return (NULL);
	while (current)
	{
		if (current->type == WORD)
		{
			if (!add_argument(current_cmd, current->value))
				return (cleanup_and_return(&command_list, current_cmd), NULL);
		}
		else if (current->type == REDIRECT_IN)
		{
			if (!handle_input_redirection(current_cmd, &current))
				return (cleanup_and_return(&command_list, current_cmd), NULL);
			continue;  // handle_input_redirection avance current
		}
		else if (current->type == REDIRECT_OUT)
		{
			if (!handle_output_redirection(current_cmd, &current, 0))
				return (cleanup_and_return(&command_list, current_cmd), NULL);
			continue;
		}
		else if (current->type == APPEND)
		{
			if (!handle_output_redirection(current_cmd, &current, 1))
				return (cleanup_and_return(&command_list, current_cmd), NULL);
			continue;
		}
		else if (current->type == HEREDOC)
		{
			if (!handle_heredoc(current_cmd, &current))
				return (cleanup_and_return(&command_list, current_cmd), NULL);
			continue;
		}
		else if (current->type == PIPE)
		{
			if (!finalize_command(&command_list, &current_cmd))
				return (cleanup_and_return(&command_list, NULL), NULL);
			current_cmd = init_command();
			if (!current_cmd)
				return (cleanup_and_return(&command_list, NULL), NULL);
		}
		current = current->next;
	}
	if (current_cmd)
		finalize_command(&command_list, &current_cmd);
	return (command_list);
}
