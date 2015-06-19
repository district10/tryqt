var net = require('net'); 
var server = net.createServer(function (socket) { 
  socket.on('data', function (data){
      console.log('connected');
          this.write("LMS Emulator.");

    this.write(data);
    console.log(data);
  }); 
}); 
server.listen(8000);