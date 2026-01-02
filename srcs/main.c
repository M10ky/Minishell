/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 21:33:27 by tarandri          #+#    #+#             */
/*   Updated: 2026/01/02 11:14:31 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/exec.h"

int g_received_signal = 0;


// int main(int argc, char **argv, char **envp)
// {
// 	t_shell	shell;

// 	(void)argc;
// 	(void)argv;
// 	shell.env = dup_env(envp);
// 	shell.last_exit_status = 0;
// 	while (1)
// 	{
// 		shell.input = readline("minishell> ");
// 		shell.tokens = tokenize(shell.input);
// 		if (check_tokens(shell.tokens))
// 		{
// 			free_tokens(shell.tokens);
// 			continue ;
// 		}
// 		expand_tokens(shell.tokens, &shell);
// 		clean_empty_tokens(&shell.tokens);
// 		shell.commands = parse(shell.tokens);
//         executor(&shell);
// 		free_tokens(shell.tokens);
// 		free_cmds(shell.commands);
// 		free(shell.input);
// 	}
// 	free_env_list(shell.env);
// 	return (0);
// }

int main(int argc, char const *argv[])
{
	(void)argc;
	(void)argv;
	// Initialize shell
	printf("Welcome to the C17 Shell!\n");
	// Main loop
	while (1)
	{
		// Read user input
		char *input = readline("minishell> ");
		if (!input)
			break;

		// Parse and execute commands
		printf("You entered: %s\n", input);
		free(input);
		// Execute commands here
		
	}
	return 0;
}
