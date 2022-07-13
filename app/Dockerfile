FROM node:latest
COPY . /usr/local/app/
WORKDIR /usr/local/app/src/
ENTRYPOINT npm start
RUN cd /usr/local/app/src/
RUN npm install
EXPOSE 8080

