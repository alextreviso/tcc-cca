const express = require('express')
    , bodyParser = require('body-parser')
    , cors = require('cors')
    , app = express();
const hello = require('./hello');

/* Middlewares */
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended: true}));
app.use(cors());

const sensors = [];

/* Routes */
app.get("/", (req, res) => {
  res.send(hello.getHelloWorld());
});

app.get('/sensors', (req, res) => res.json(sensors));

app.get('/sensors/:id', (req, res) => {
  const id = parseInt(req.params.id);
  const sensor = sensors.filter((item) => item.id === id);

  if (sensor.length <= 0) {
    return res.json({});
  }
  return res.json(sensor[0]);
});

app.post('/sensors', (req, res) => {
  const sensor = {
    'id': req.body.id,
    'isIn': req.body.isIn,
    'time': req.body.time,
  };
  sensors.push(sensor);

  res.json(sensor);
});

/* App listen */
app.listen(3000, () => {
  console.log(`nodejs-backend is running`);
  console.log(`open in http://127.0.0.1:3000/sensors`);
});
