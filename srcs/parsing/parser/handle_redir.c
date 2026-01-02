/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tarandri <tarandri@student.42antananarivo. +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 14:57:18 by tarandri          #+#    #+#             */
/*   Updated: 2026/01/02 09:38:39 by tarandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/parsing.h"

int	handle_input_redirection(t_command *cmd, t_token **current)
{
	*current = (*current)->next;
	if (!*current || (*current)->type != WORD)
		return (0);
	if (cmd->input_redirection)
		free(cmd->input_redirection);
	cmd->input_redirection = ft_strdup((*current)->value);
	if (!cmd->input_redirection)
		return (0);
	*current = (*current)->next;
	return (1);
}

int	handle_output_redirection(t_command *cmd, t_token **current, int append)
{
	*current = (*current)->next;
	if (!*current || (*current)->type != WORD)
		return (0);
	if (cmd->output_redirection)
		free(cmd->output_redirection);
	cmd->output_redirection = ft_strdup((*current)->value);
	if (!cmd->output_redirection)
		return (0);
	cmd->append_output = append;
	*current = (*current)->next;
	return (1);
}

int	handle_heredoc(t_command *cmd, t_token **current)
{
	*current = (*current)->next;
	if (!*current || (*current)->type != WORD)
		return (0);
	if (cmd->input_redirection)
		free(cmd->input_redirection);
	cmd->input_redirection = ft_strdup((*current)->value);
	if (!cmd->input_redirection)
		return (0);
	cmd->is_heredoc = 1;
	*current = (*current)->next;
	return (1);
}
