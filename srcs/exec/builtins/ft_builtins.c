/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_builtins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 21:31:02 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/11 22:58:25 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/exec.h"

static int	is_valid_exit_arg(char *str)
{
	int	i;

	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (!ft_isdigit(str[i]))
		return (0);
	while (ft_isdigit(str[i]))
		i++;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] != '\0')
		return (0);
	return (1);
}

static int	is_n_flag(char *arg)
{
	int	i;

	i = 1;
	if (ft_strncmp(arg, "-n", 2) != 0)
		return (0);
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

int	builtin_echo(char **args)
{
	int	i;
	int	n_flag;

	i = 1;
	n_flag = 0;
	while (args[i] && is_n_flag(args[i]))
	{
		n_flag = 1;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], 1);
		if (args[i + 1])
			ft_putstr_fd(" ", 1);
		i++;
	}
	if (!n_flag)
		ft_putstr_fd("\n", 1);
	return (0);
}

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		printf("%s\n", cwd);
		free(cwd);
		return (0);
	}
	else
	{
		perror("minishell: pwd");
		return (1);
	}
}

static int	handle_too_many_args(void)
{
	ft_putstr_fd("minishell: exit: too many arguments\n", 2);
	return (1);
}
int	ft_atoll_safe(const char *str, long long *result)
{
	unsigned long long	res;
	int					sign;
	int					i;

	res = 0;
	sign = 1;
	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i++] == '-')
			sign = -1;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (sign == 1 && (res > LLONG_MAX / 10 || (res == LLONG_MAX / 10
					&& (str[i] - '0') > LLONG_MAX % 10)))
			return (-1);
		if (sign == -1 && (res > (unsigned long long)LLONG_MAX + 1 / 10
				|| (res == ((unsigned long long)LLONG_MAX + 1) / 10 && (str[i]
						- '0') > 8)))
			return (-1);
		res = res * 10 + (str[i++] - '0');
	}
	*result = res * sign;
	return (0);
}
static void	exit_with_error(char *arg, t_shell *shell, char **args_array)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
	free(args_array);
	if (shell->commands->next)
		cleanup_child(shell);
	else
		cleanup_shell(shell);
	exit(2);
}

int	builtin_exit(char **args, t_shell *shell)
{
	long long	exit_code;

	if (!shell->commands->next)
		ft_putstr_fd("exit\n", 2);
	if (args[1])
	{
		if (!is_valid_exit_arg(args[1]) || ft_atoll_safe(args[1], &exit_code) ==
			-1)
			exit_with_error(args[1], shell, args);
		if (args[2])
		{
			// free(args);
			return (handle_too_many_args());
		}
	}
	else
		exit_code = shell->last_exit_status;
	free(args);
	if (shell->commands->next)
		cleanup_child(shell);
	else
		cleanup_shell(shell);
	exit(exit_code % 256);
}

int	builtin_env(t_env *env)
{
	while (env)
	{
		printf("%s=%s\n", env->key, env->value);
		env = env->next;
	}
	return (0);
}

static int	cd_error_no_home(char *old_pwd)
{
	ft_putstr_fd("minishell: cd: HOME not set\n", 2);
	free(old_pwd);
	return (1);
}

static int	cd_error_chdir(char *dir, char *old_pwd)
{
	ft_putstr_fd("minishell: cd: ", 2);
	ft_putstr_fd(dir, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putstr_fd("\n", 2);
	free(old_pwd);
	return (1);
}

static char	*build_absolute_path(char *current_pwd, char *dir)
{
	char	*temp;
	char	*result;

	if (!current_pwd || !dir)
		return (NULL);

	/* Si dir est vide ou ".", rester dans current_pwd */
	if (dir[0] == '\0' || (dir[0] == '.' && dir[1] == '\0'))
		return (ft_strdup(current_pwd));

	/* Construire : current_pwd + "/" + dir */
	temp = ft_strjoin(current_pwd, "/");
	if (!temp)
		return (NULL);
	result = ft_strjoin(temp, dir);
	free(temp);
	return (result);
}

/* ========== FONCTION AUXILIAIRE : Normaliser un chemin ========== */

static char	*normalize_path(char *path)
{
	char	**parts;
	char	**stack;
	int		i;
	int		j;
	char	*result;
	char	*temp;

	if (!path || path[0] != '/')
		return (ft_strdup(path));

	parts = ft_split(path, '/');
	if (!parts)
		return (ft_strdup(path));

	/* Compter les segments */
	i = 0;
	while (parts[i])
		i++;

	stack = malloc(sizeof(char *) * (i + 1));
	if (!stack)
	{
		free_tab(parts);
		return (ft_strdup(path));
	}

	/* Résoudre . et .. */
	i = 0;
	j = 0;
	while (parts[i])
	{
		if (ft_strcmp(parts[i], "..") == 0)
		{
			if (j > 0)
				j--;  /* Remonter d'un niveau */
		}
		else if (ft_strcmp(parts[i], ".") != 0 && parts[i][0] != '\0')
		{
			stack[j++] = parts[i];
		}
		i++;
	}

	/* Reconstruire le chemin */
	if (j == 0)
		result = ft_strdup("/");
	else
	{
		result = ft_strdup("");
		i = 0;
		while (i < j)
		{
			temp = ft_strjoin(result, "/");
			free(result);
			result = ft_strjoin(temp, stack[i]);
			free(temp);
			i++;
		}
	}

	free(stack);
	free_tab(parts);
	return (result);
}

static void	update_pwd_vars(t_env *env, char *old_pwd, char *new_dir)
{
	char	*new_pwd;
	char	*temp;

	update_env_var(env, "OLDPWD", old_pwd);

	/* Chemin absolu : normaliser et utiliser */
	if (new_dir && new_dir[0] == '/')
	{
		new_pwd = normalize_path(new_dir);
	}
	/* Chemin relatif : construire depuis PWD actuel */
	else if (new_dir)
	{
		temp = build_absolute_path(old_pwd, new_dir);
		if (temp)
		{
			new_pwd = normalize_path(temp);
			free(temp);
		}
		else
			new_pwd = getcwd(NULL, 0);
	}
	/* Fallback */
	else
	{
		new_pwd = getcwd(NULL, 0);
	}

	if (new_pwd)
	{
		update_env_var(env, "PWD", new_pwd);
		free(new_pwd);
	}
}


int	builtin_cd(char **args, t_env *env)
{
	char	*dir;
	char	*old_pwd;

	/* Récupérer PWD depuis l'environnement (chemin logique) */
	old_pwd = get_env_value(env, "PWD");
	if (!old_pwd)
		old_pwd = getcwd(NULL, 0);
	else
		old_pwd = ft_strdup(old_pwd);

	if (!old_pwd)
	{
		perror("minishell: cd: getcwd");
		return (1);
	}

	/* Déterminer le répertoire cible */
	if (!args || !args[1])
		dir = get_env_value(env, "HOME");
	else
		dir = args[1];

	if (!dir)
		return (cd_error_no_home(old_pwd));

	/* Effectuer le changement de répertoire */
	if (chdir(dir) != 0)
		return (cd_error_chdir(dir, old_pwd));

	/* Mettre à jour PWD et OLDPWD */
	update_pwd_vars(env, old_pwd, dir);
	free(old_pwd);
	return (0);
}
