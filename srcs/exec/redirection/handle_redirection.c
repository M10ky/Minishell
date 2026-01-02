/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirection.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:32:52 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/02 10:38:08 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/exec.h"


static int	handle_input_redir(char *input_file)
{
	int	fd;

	fd = open(input_file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(input_file);
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	open_output_file(char *output_file, int append)
{
	int	fd;

	if (append)
		fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	return (fd);
}

static int	handle_output_redir(char *output_file, int append)
{
	int	fd;

	fd = open_output_file(output_file, append);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		perror(output_file);
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

int	handle_redirections(t_command *cmd)
{
	if (!cmd)
		return (0);
	if (cmd->input_redirection)
	{
		if (handle_input_redir(cmd->input_redirection))
			return (1);
	}
	if (cmd->output_redirection)
	{
		if (handle_output_redir(cmd->output_redirection, cmd->append_output))
			return (1);
	}
	return (0);
}
