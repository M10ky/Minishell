/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 14:31:40 by tarandri          #+#    #+#             */
/*   Updated: 2026/01/02 11:02:01 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/parsing.h"

static void	free_cmd(t_command *cmd)
{
	int	i;

	i = 0;
	if (!cmd)
		return ;
	if (cmd->args)
	{
		while (cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	if (cmd->input_redirection)
		free(cmd->input_redirection);
	if (cmd->output_redirection)
		free(cmd->output_redirection);
	free(cmd);
}
int	cleanup_and_return(t_command **cmd_list, t_command *current_cmd)
{
	free_cmds(*cmd_list);
	free_cmds(current_cmd);
	return (0);
}

void	free_cmds(t_command *cmds)
{
	t_command	*tmp;

	while (cmds)
	{
		tmp = cmds;
		cmds = cmds->next;
		free_cmd(tmp);
	}
}
