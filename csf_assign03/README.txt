TODO: names of team members and their contributions to the project

TODO (for MS3): best cache report

Observations:
1. lru vs fifo
> while lru is only marginally better than fifo (only better by a few hits in a few tests), 
fifo requires much less overhead thus fifo is probably the better choice

2. direct vs fully associative
> while direct mapped had a more misses than fully associative, it took signficantly
longer to run fully associative

3. write-allocate vs no-write-allocate
> write-allocate led to noticeably more hits than no-wrote-allocate

4. write-back vs write-through
> write-back and write-through had basically the same hit and miss rate
> however, write-back took signficantly fewer clock cycles to run

5. block size
> larger block size meant more hits but also more clock cycles

1.1 fully associative lru
./csim 1 32768 64 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 25651483

./csim 1 32768 64 no-write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315897
Load misses: 2300
Store hits: 173709
Store misses: 23777
Total cycles: 23920506

./csim 1 32768 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5902883

1.2 fully associative fifo
./csim 1 32768 64 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 25651483

./csim 1 32768 64 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315897
Load misses: 2300
Store hits: 173709
Store misses: 23777
Total cycles: 23920506

./csim 1 32768 64 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5902883

2.1 direct mapped lru
./csim 32768 1 64 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317171
Load misses: 1026
Store hits: 195095
Store misses: 2391
Total cycles: 25731403

./csim 32768 1 64 no-write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315880
Load misses: 2317
Store hits: 173691
Store misses: 23795
Total cycles: 23947621

./csim 32768 1 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317171
Load misses: 1026
Store hits: 195095
Store misses: 2391
Total cycles: 6107603

2.2 direct mapped fifo
./csim 32768 1 64 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317171
Load misses: 1026
Store hits: 195095
Store misses: 2391
Total cycles: 25731403

./csim 32768 1 64 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315880
Load misses: 2317
Store hits: 173691
Store misses: 23795
Total cycles: 23947621

./csim 32768 1 64 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317171
Load misses: 1026
Store hits: 195095
Store misses: 2391
Total cycles: 6107603

3.1 8 way set lru
./csim 4096 8 64 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317201
Load misses: 996
Store hits: 195110
Store misses: 2376
Total cycles: 25659472

./csim 4096 8 64 no-write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315896
Load misses: 2301
Store hits: 173709
Store misses: 23777
Total cycles: 23922098

./csim 4096 8 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317201
Load misses: 996
Store hits: 195110
Store misses: 2376
Total cycles: 5949272

3.2 8 way set fifo
./csim 4096 8 64 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317199
Load misses: 998
Store hits: 195110
Store misses: 2376
Total cycles: 25662670

./csim 4096 8 64 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315896
Load misses: 2301
Store hits: 173709
Store misses: 23777
Total cycles: 23922098

./csim 4096 8 64 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317199
Load misses: 998
Store hits: 195110
Store misses: 2376
Total cycles: 5955670

4.1 16 way set associative lru
./csim 2048 16 64 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 25651481

./csim 2048 16 64 no-write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315897
Load misses: 2300
Store hits: 173709
Store misses: 23777
Total cycles: 23920505

./csim 2048 16 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5907681

4.2 16 way set associative fifo
./csim 2048 16 64 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317203
Load misses: 994
Store hits: 195111
Store misses: 2375
Total cycles: 25654680

./csim 2048 16 64 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315897
Load misses: 2300
Store hits: 173709
Store misses: 23777
Total cycles: 23920505

./csim 2048 16 64 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317203
Load misses: 994
Store hits: 195111
Store misses: 2375
Total cycles: 5914080

5.1 32 way set associative lru
./csim 1024 32 64 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 25651483

./csim 1024 32 64 no-write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315897
Load misses: 2300
Store hits: 173709
Store misses: 23777
Total cycles: 23920506

./csim 1024 32 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5902883

5.2 32 way set associative fifo
./csim 1024 32 64 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 25651483

./csim 1024 32 64 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315897
Load misses: 2300
Store hits: 173709
Store misses: 23777
Total cycles: 23920506

./csim 1024 32 64 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5902883

6. testing block size (using 8 way, write-allocate write-back)

./csim 2048 8 128 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317531
Load misses: 666
Store hits: 196198
Store misses: 1288
Total cycles: 6845272

./csim 4096 8 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317201
Load misses: 996
Store hits: 195110
Store misses: 2376
Total cycles: 5949272

./csim 8192 8 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316692
Load misses: 1505
Store hits: 192953
Store misses: 4533
Total cycles: 5364473

./csim 16384 8 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315854
Load misses: 2343
Store hits: 188614
Store misses: 8872
Total cycles: 5010874

./csim 2048 8 128 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317529
Load misses: 668
Store hits: 196197
Store misses: 1289
Total cycles: 6861270

./csim 4096 8 64 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317199
Load misses: 998
Store hits: 195110
Store misses: 2376
Total cycles: 5955670

./csim 8192 8 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316691
Load misses: 1506
Store hits: 192953
Store misses: 4533
Total cycles: 5366072

./csim 16384 8 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315854
Load misses: 2343
Store hits: 188614
Store misses: 8872
Total cycles: 5010874
