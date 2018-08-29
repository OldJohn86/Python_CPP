#!/bin/sh
if [ ! -e gitdir-index ]; then
        echo "WARNINING: no gitdir-index file found, regenerating.."
        rm gitdir-index
        find . -name .git -printf %h\\n >> gitdir-index
fi
 
if [ "$1" = '-f' ]; then
        rm gitdir-index
        echo "forcing regeneration of existing gitdir-index file, overwritting .."
        find . -name .git -printf %h\\n >> gitdir-index
fi
 
grep -E 'ca-cli/|ca-iros/|ca-network-engine/|ca-plat/|ca-pon-mac/|ca-scfg/|ca-app/|ca-oam/|ca-omci/|libca-app/' gitdir-index > 1.txt
for GITDIR in $(cat 1.txt)
do
        echo "updating $GITDIR"
        (cd $GITDIR; git fetch -p; git pull)
done
