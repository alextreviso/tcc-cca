#!/bin/bash

app_name=$1

aws ecr get-login-password --region us-east-1 | docker login --username AWS --password-stdin 657495107958.dkr.ecr.us-east-1.amazonaws.com

docker build -t ${app_name} .

docker tag ${app_name}:latest 657495107958.dkr.ecr.us-east-1.amazonaws.com/${app_name}:latest

docker push 657495107958.dkr.ecr.us-east-1.amazonaws.com/${app_name}:latest