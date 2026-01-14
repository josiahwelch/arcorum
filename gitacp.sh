#1/bin/sh
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/*
git add * && git commit && git push
