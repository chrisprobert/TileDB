CREATE TABLE dense_50000x20000_2500x1000_withtileid_withrle (
X         INTEGER NOT NULL ENCODING RLE,
Y         INTEGER NOT NULL ENCODING RLE,
TID       INTEGER NOT NULL ENCODING RLE,
A1        INTEGER NOT NULL ENCODING GZIP_COMP,
PRIMARY KEY (TID,X,Y)
);