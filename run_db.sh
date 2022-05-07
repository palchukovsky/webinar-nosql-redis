docker run \
--rm \
-d \
--name redis-demo-server \
--ulimit nofile=262144:262144 \
-p 0.0.0.0:6379:6379 \
redis
