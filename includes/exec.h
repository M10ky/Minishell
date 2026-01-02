/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 11:01:22 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/02 10:52:23 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H
# define BUFFER_SIZE 1024
# include "parsing.h"

char *get_path(t_env *env, char *cmd);
t_command *create_test_cmd(char **args);
char **env_to_tab(t_env *env);
void    exec_simple_cmd(t_command *cmd, t_env *env);
void    executor(t_shell *shell);
void init_test_data(t_shell *shell);
void init_test_data2(t_shell *shell);
int handle_redirections(t_command *cmd);
void init_env(t_shell *shell, char **envp);
char *get_env_value(t_env *env, char *key);
void free_tab(char **tab);
int count_env_vars(t_env *env);
int	is_builtin(char *cmd);
int	execute_builtin(t_command *cmd, t_shell *shell);
int builtin_echo(char **args);
int builtin_pwd(void);
int builtin_exit(char **args, t_shell *shell);
int builtin_env(t_env *env);
int builtin_unset(char **args, t_env **env);
int builtin_cd(char **args, t_env *env);
int builtin_export(char **args, t_env **env);
void update_env_var(t_env *env, char *key, char *new_value);
void free_env(t_env *env);
void free_command(t_command *cmd);
void free_commands(t_command *commands);
void cleanup_shell(t_shell *shell);
void    sig_handler_exec(int sig);
void    setup_exec_signals(void);
void    setup_child_signals(void);
void	exp_add_env_node_back(t_env **head, t_env *new_node);
void free_tab_partial(char **tab, int count);
int	ft_unset(t_shell *shell, char **args);
int	ft_export(t_shell *shell, char **args);
int create_heredoc_file(char *delimiter, char *filename);
int process_heredocs(t_shell *shell);
void cleanup_heredocs(t_shell *shell);
void sig_handler_heredoc(int sig);
int check_parent_builtin(char *cmd, t_shell *shell);
void setup_prompt_signal(void);
void exec_builtin_parent(t_command *cmd, t_shell *shell);
void expand_heredoc_line(char *line, int fd, t_shell *shell);
int check_parent_builtin(char *cmd, t_shell *shell);
char	*get_next_line(int fd);
// void wait_all_children(pid_t last_pid, t_shell *shell);
void setup_heredoc_signals(void);
void cleanup_child(t_shell *shell);
#endif
