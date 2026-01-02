/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miokrako <miokrako@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 15:17:38 by miokrako          #+#    #+#             */
/*   Updated: 2026/01/02 10:58:38 by miokrako         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/exec.h"



// int g_received_signal = 0;

// Handler pour Ctrl+C pendant le prompt
void sig_handler_prompt(int sig)
{
    (void)sig;
    g_received_signal = SIGINT;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

// Handler pour Ctrl+C et Ctrl+\ pendant execution
void sig_handler_exec(int sig)
{
    if (sig == SIGINT)
    {
        g_received_signal = SIGINT;
        write(1, "\n", 1);
    }
    else if (sig == SIGQUIT)
    {
        g_received_signal = SIGQUIT;
        ft_putstr_fd("Quit (core dumped)\n", 2);
    }
}

// Handler pour Ctrl+C pendant heredoc
void sig_handler_heredoc(int sig)
{
    if (sig == SIGINT)
    {
        g_received_signal = SIGINT;
        close(STDIN_FILENO);
    }
    // NOTE: SIGQUIT n'est pas géré ici car il est ignoré
}

// Setup pour le prompt interactif
void setup_prompt_signal(void)
{
    signal(SIGINT, sig_handler_prompt);  // Ctrl+C → Nouvelle ligne + prompt
    signal(SIGQUIT, SIG_IGN);            // Ctrl+\ → Ignoré
}

// Setup pendant l'execution (parent)
void setup_exec_signals(void)
{
    signal(SIGINT, sig_handler_exec);   // Ctrl+C → Afficher \n
    signal(SIGQUIT, sig_handler_exec);  // Ctrl+\ → "Quit (core dumped)"
}

// Setup pour les processus enfants (comportement par défaut)
void setup_child_signals(void)
{
    signal(SIGINT, SIG_DFL);   // Comportement par défaut
    signal(SIGQUIT, SIG_DFL);  // Comportement par défaut
}

// ✅ NOUVEAU: Setup spécifique pour heredoc
void setup_heredoc_signals(void)
{
    signal(SIGINT, sig_handler_heredoc);  // Ctrl+C → Arrêter heredoc
    signal(SIGQUIT, SIG_IGN);             // Ctrl+\ → IGNORER (comme bash)
}
