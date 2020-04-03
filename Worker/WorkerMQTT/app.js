var mqtt = require('mqtt')
const amqp = require('amqplib//callback_api')
const Configuration = require('./Model/config')
const mongo = require('mongoose')
const parseArgs = require('minimist')
let args = parseArgs(process.argv.slice(2), { string: ['command'] })
let Mac = args.mac
let command = args.command
let durasi = args.duration
var dataValue
let jadwal = args.jadwal
var numberArrayOn
var numberArrayOff
var splitData
var valueData
var splitdataOn
var splitdataOff


//connect mongodb 
mongo.connect('mongodb://homeauto2:hom3auto2020!@dbmongo.server.pptik.id:27017/homeauto2', {
  useNewUrlParser: true,
  useUnifiedTopology: true
})

const db = mongo.connection
db.on('error', error => console.error(error)
)
db.once('open', () => console.log('Berhasil Terhubung ke database'))

//temporare data from db
var dataTemp = ""

//connect rmq
var client = mqtt.connect('mqtt://167.205.7.226', {
  username: '/homeauto:homeauto',
  password: 'homeauto12345!',
  port: 1883
})

//check db to the last data from database
Configuration.findOne({ mac: Mac }).sort({ $natural: -1 }).then(data => {
  dataTemp = data
  valueData = dataTemp.value
  splitData = valueData.split("")
  console.log(splitData)

  // check incoming message to parsing each element of  array from schedule


  //if schedule to turn on device 
  if (jadwal == "1") {
    splitdataOn = command.split("")
    numberArrayOn = (splitdataOn.indexOf('1'))
    console.log(numberArrayOn)

    for (i = 0; i < splitData; i++) {
      splitData[i] = splitData[i - 1]

    }
    splitData[numberArrayOn] = "1"

    splitData = splitData
    console.log("on")

  }
  //if schedule to turn off device 
  if (jadwal == "0") {
    splitdataOff = command.split("")
    numberArrayOff = (splitdataOff.indexOf('0'))
    console.log(numberArrayOff)
    for (i = 0; i < splitData; i++) {
      splitData[i] = splitData[i - 1]

    }
    splitData[numberArrayOff] = "0"

    console.log(splitData)
    splitData = splitData
    console.log("off")
  }


  var messageCommand = splitData.join("")
  console.log(messageCommand)
  //publish message
  client.publish(Mac, messageCommand)

})

setTimeout((function () {
  process.exit()
}), 10000);