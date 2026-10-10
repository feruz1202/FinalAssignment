# This file lists all the short comings of our project compared to real Redis.

## Thread per connection
 - Spawning a seperate thread for each TCP connection is inefficient 
### Solution:
  Should use a thread pool.

## Lack of incremental parsing
 - One 4 KB buffer read to parse RESP bytes.
### Solution 
  Should implement an incremental parser
## Memory consumpstion (using strings)
  Currently storing only strings
### Solution
  should store arbitary binary data disregarding the type
