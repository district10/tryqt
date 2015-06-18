# Qt Code Snippets


```c++
QString filename = "/tmp/data.txt";
QFile file( filename );
if ( file.open(QIODevice::ReadWrite) ) {
    QTextStream stream( &file );
    stream.setCodec("UTF-8");
    stream << "something" << endl;
}
```
