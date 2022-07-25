const express =  require('express');
const app = express();
const hello = require('./hello');
const employee = require('./employee')

app.get("/", (req, res) => {
    res.send(hello.getHelloWorld());
});

app.get("/RegisterEmployee", (req, res) => {
    res.send(employee.register())
});

app.listen(8080, () => {
    console.log("Aplicação iniciada na porta 8080: http://localhost:8080");
})