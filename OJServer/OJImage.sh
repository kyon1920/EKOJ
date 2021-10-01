docker run image-$1:latest > ./result/$1.result
sleep 1s
docker kill -s KILL image-$1:latest
docker rmi image-$1:latest --force
docker rmi $(docker images -f "dangling=true" -q) --force