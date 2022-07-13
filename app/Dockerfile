FROM node:latest
COPY . /usr/local/app/
WORKDIR /usr/local/app/src/
RUN cd /usr/local/app/src/
RUN npm install
EXPOSE 8080
ENTRYPOINT npm start

