#!/bin/bash

# Tests spécifiques pour les cas limites qui peuvent causer des fuites

echo "=== TESTS EDGE CASES CRITIQUES ==="
echo

# Test 1: Heredoc interrompu par Ctrl+C
echo "Test 1: Heredoc avec timeout (simuler Ctrl+C)"
echo "Tapez Ctrl+C pendant le heredoc"
timeout 2 valgrind --leak-check=full --log-file=valgrind_heredoc_interrupt.log ./minishell << 'EOF'
cat << END
ligne 1
ligne 2
EOF
# Le timeout va simuler une interruption
grep "definitely lost\|indirectly lost" valgrind_heredoc_interrupt.log
rm -f valgrind_heredoc_interrupt.log

# Test 2: Export avec malloc failure simulé
echo
echo "Test 2: Multiples exports/unsets en boucle"
valgrind --leak-check=full --log-file=valgrind_export.log ./minishell << 'EOF'
export A=1
export B=2
export C=3
export D=4
export E=5
export F=6
export G=7
export H=8
export I=9
export J=10
unset A
unset B
unset C
unset D
unset E
unset F
unset G
unset H
unset I
unset J
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_export.log
rm -f valgrind_export.log

# Test 3: Pipe avec builtin qui échoue
echo
echo "Test 3: Pipe avec cd (builtin qui ne doit pas être dans pipe)"
valgrind --leak-check=full --log-file=valgrind_pipe_builtin.log ./minishell << 'EOF'
echo test | cd /tmp | cat
pwd
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_pipe_builtin.log
rm -f valgrind_pipe_builtin.log

# Test 4: Redirections multiples qui échouent
echo
echo "Test 4: Redirections vers fichiers inexistants"
valgrind --leak-check=full --log-file=valgrind_redir_fail.log ./minishell << 'EOF'
cat < /fichier/inexistant
echo test > /dossier/inexistant/fichier
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_redir_fail.log
rm -f valgrind_redir_fail.log

# Test 5: Expansion avec variables vides
echo
echo "Test 5: Expansion de variables vides et inexistantes"
valgrind --leak-check=full --log-file=valgrind_expand.log ./minishell << 'EOF'
export EMPTY=
echo $EMPTY
echo $INEXISTANT
echo "$EMPTY"
echo '$EMPTY'
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_expand.log
rm -f valgrind_expand.log

# Test 6: Commande qui n'existe pas
echo
echo "Test 6: Exécution de commande inexistante"
valgrind --leak-check=full --log-file=valgrind_notfound.log ./minishell << 'EOF'
commandeinexistante
/chemin/inexistant/commande
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_notfound.log
rm -f valgrind_notfound.log

# Test 7: Exit avec différents arguments
echo
echo "Test 7: Exit avec arguments invalides"
valgrind --leak-check=full --log-file=valgrind_exit1.log ./minishell << 'EOF'
exit abc
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_exit1.log
rm -f valgrind_exit1.log

valgrind --leak-check=full --log-file=valgrind_exit2.log ./minishell << 'EOF'
exit 999999999999999999999
EOF
grep "definitely lost\|indirectly lost" valgrind_exit2.log
rm -f valgrind_exit2.log

valgrind --leak-check=full --log-file=valgrind_exit3.log ./minishell << 'EOF'
exit 1 2 3
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_exit3.log
rm -f valgrind_exit3.log

# Test 8: Heredoc avec expansion complexe
echo
echo "Test 8: Heredoc avec expansion de variables"
valgrind --leak-check=full --log-file=valgrind_heredoc_expand.log ./minishell << 'EOF'
export VAR=test
cat << EOF2
$VAR
$?
$INEXISTANT
EOF2
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_heredoc_expand.log
rm -f valgrind_heredoc_expand.log

# Test 9: Multiples heredocs successifs
echo
echo "Test 9: Multiples heredocs"
valgrind --leak-check=full --log-file=valgrind_multi_heredoc.log ./minishell << 'EOF'
cat << EOF1 << EOF2
first
EOF1
second
EOF2
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_multi_heredoc.log
rm -f valgrind_multi_heredoc.log

# Test 10: Quotes non fermées (erreur de syntaxe)
echo
echo "Test 10: Quotes non fermées"
valgrind --leak-check=full --log-file=valgrind_unclosed_quote.log ./minishell << 'EOF'
echo "test
echo 'test
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_unclosed_quote.log
rm -f valgrind_unclosed_quote.log

# Test 11: Très longue ligne
echo
echo "Test 11: Ligne très longue"
valgrind --leak-check=full --log-file=valgrind_long_line.log ./minishell << 'EOF'
echo aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_long_line.log
rm -f valgrind_long_line.log

# Test 12: Pipe cassé
echo
echo "Test 12: Pipe vers commande qui échoue"
valgrind --leak-check=full --log-file=valgrind_broken_pipe.log ./minishell << 'EOF'
yes | head -1
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_broken_pipe.log
rm -f valgrind_broken_pipe.log

# Test 13: CD avec chemin invalide
echo
echo "Test 13: CD vers répertoire inexistant"
valgrind --leak-check=full --log-file=valgrind_cd_fail.log ./minishell << 'EOF'
cd /dossier/inexistant
cd
cd /tmp
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_cd_fail.log
rm -f valgrind_cd_fail.log

# Test 14: Redirection ambiguë
echo
echo "Test 14: Redirection ambiguë avec expansion"
valgrind --leak-check=full --log-file=valgrind_ambiguous.log ./minishell << 'EOF'
export FILES="file1 file2"
echo test > $FILES
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_ambiguous.log
rm -f valgrind_ambiguous.log

# Test 15: Fichier script non exécutable
echo
echo "Test 15: Exécuter un fichier non exécutable"
echo "#!/bin/bash
echo test" > /tmp/test_script.sh
chmod -x /tmp/test_script.sh
valgrind --leak-check=full --log-file=valgrind_noexec.log ./minishell << 'EOF'
/tmp/test_script.sh
exit
EOF
grep "definitely lost\|indirectly lost" valgrind_noexec.log
rm -f /tmp/test_script.sh valgrind_noexec.log

echo
echo "=== FIN DES TESTS EDGE CASES ==="
