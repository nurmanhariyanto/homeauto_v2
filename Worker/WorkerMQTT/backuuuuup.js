var mqtt = require('mqtt')
const amqp = require('amqplib//callback_api')
var MongoClient = require('mongodb').MongoClient
var nomgodb_uri = "mongodb://homeauto2:hom3auto2020!@dbmongo.server.pptik.id:27017/homeauto2";
var dbName = 'homeauto2'
const parseArgs = require('minimist')
let args = parseArgs(process.argv.slice(2), {string: ['command']})
let Mac = args.mac
let durasi = args.duration
let jadwal = args.jadwal

function sendToMqtt () {
  //connect rmq
  var client = mqtt.connect('mqtt://167.205.7.226/mqtt', {
    username: '/homeauto:homeauto',
    password: 'homeauto12345!',
    protocolId: 'MQTT'
  })

  //publish
  client.on('connect', function () {
    // node app --mac XXXX:XXXX:XXXX:XXXX --command baru
  client.publish(Mac, args.command)
    // console.log("berhasil kirim " + pub)
    client.end()
  })
}
sendToMqtt()

amqp.connect({ protocol: 'amqp', hostname: '167.205.7.226', port: '5672', username: 'homeauto', password: 'homeauto12345!', vhost: '/homeauto' }, (err, conn) => {
    conn.createChannel((err, ch) => {
        const q = 'Publish' //Nama queue untuk amq
        ch.assertQueue(q, { durable: true }) //menyatakan queue nya bernama task_queue
        ch.prefetch(1)
        console.log(`[*] Menunggu pesan di %s. untuk keluar tekan CTRL + C`, q)
        //Menangkap pesan yang dikirimkan rabbitmq dari antrian

        ch.consume(q, msg => {
            let message = msg.content.toString()
            var today = new Date()
            let msgg = JSON.parse(message);
            let mac = msgg.mac
            let pesan = msgg.value
            // const secs = msg.content.toString().split('.').length - 1

  try {
  //callback
  // Connection To Mongodb
  MongoClient.connect(nomgodb_uri, {useNewUrlParser: true, useUnifiedTopology: true})
    .then(function(client){
      var clientDB = client.db(dbName)
      // Insert to history DB
      clientDB.collection("historys").insertOne({"mac": mac ,"pesan" : pesan, "hari" : today})
    })
    .catch(function(err){ console.log(err) })
} catch (error) {
  console.log(error)
}



            console.log(`[x] Menerima %s`, message.toString() + today)
            console.log(`[x] Done`)

            //Ack dan noAck untuk mengantisipasi saat koneksi mati queue masih tersimpan dan belum di hapus
            ch.ack(msg)
        })
        
    })
})

setTimeout((function() {
  process.exit()
}),5000);


