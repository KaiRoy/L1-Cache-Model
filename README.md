# L1-Cache-Model
Modeling an L1 Cache as a Final Project for ECE 585 - Microprocessor System Design

<!-- Insert Image Here -->

<!-- LINK SUMMARY VIDEO SOMEWHERE IN THE README!! -->

## Table of Contents

- [Description](#description)
- [Design](#design)
- [Current State](#current-state-of-the-project)
- [Post Project Notes](#post-project-notes)
- [Installation](#installation) <!-- Should I have Installation and Usage above or below the Design, Current State, Changes, etc? -->
- [Usage](#usage)
- [Credits](#credits)
- [License](#license)

## Description

In this project, we created a model of a Level 1 Cache with a simulated MESI communication output.

This project consisted of our team designing and building a testable model of a L1 split cache for a 32-bit processor that uses the MESI protocol. The cache is split into two parts: An instruction cache and data cache. A trace file will be telling us which cache that we are accessing. We were given some requirements to follow for our model which consisted of the following: 

| Cache type | Instruction | Data |
| ----------- | ----------- | ----------- |
| Associativity | Four-way set associative | Eight-way set associative |
| Number of Sets | 16K Sets | 16K Sets |
| Byte lines | 64-lines | 64-lines |
| Replacement policy | LRU Replacement | LRU Replacement |

Both caches will be backed by a shared L2 Cache and will be using the MESI protocol to ensure cache coherence. 

<!-- Structure -->
<!-- Block Diagram -->	

<!-- Instruction Format -->


<!-- MESI Format-->
**MESI Protocol:**
To ensure that we keep a good cache coherence, we will be using one of the most widely used coherence protocols called the “MESI Protocol”. The MESI is an acronym that stands for the 4 states that a cache line can be marked with using two additional bits.

- **M** : Modified means that the value in the cache is considered dirty. This means that this value is different from what is currently stored in main memory and no other processor caches contain the line. Once the data has been written back to main memory, this will change the state to S or Shared. 
- **E** : Exclusive The value in the cache line is the same as the value in main memory meaning that it's clean and also no other processor has a copy of the cache item. Can be changed to shared state from a read or to modified state from a write. 
- **S** : Shared means this cache line is stored in other caches and holds the most recent data copy and is considered clean meaning it is the same in main memory. 
- **I** : Invalid means the current cache block is invalid and needs to be fetched from another cache or main memory since it does not hold a copy of the line. 

<!-- Output Format/Modes -->
**Output Modes:**
The model requires the implementation of two different output modes. Mode 0 will simply summarize our usage statistics and provide responses to 9’s from the trace file. Mode 1 will display the simulated MESI communication to an L2 cache as well as all of the statistics from mode 0. 

At the end of the simulation, the model will need to ouptut the following statistics:
- Number of cache reads
- Number of cache writes
- Number of cache hits
- Number of cache misses
- Cache hit ratio


<!-- Insert Image of Format? -->


## Designs

<!-- Create a Block Diagram of the System?-->
<!-- ![System Block Diagram](Assets/ECE%2044x%20Block%20Diagram.png) -->

<!-- Design Implementation Choices-->
From the required specifications of our device we can assume the following for dividing up our cache addresses.


**Cache Addressing:**
- 32-bit processor = 32 address bits
- \# of byte select bits (64 bytes /cache line)  = log2(M) = log2(64) = 6 bits
- Cache contains 16K sets = 2^10 * 2^4 = 14 bits
- \# of bits for tag = 32 - 6 - 14 = 12 bits

| --- | --- | --- |
| Address Tag [31:20] | Set Bits [19:6] | Offset [5:0] |



**L1 Split Cache:**
Since we are modeling a split cache, we have 2 separate caches, one for instructions and the other for data. The “Instruction cache” handles information going to the processor while the “Data cache” holds information to be written to memory. The level one indicates that this is the first level of cache that is present inside the processor. This level of cache typically ranges from 2KB to 64KB. 

**L2 Cache:**
This is our secondary memory cache that will usually be larger than the L1 Cache, but run at a slower speed. These types of caches can range from 256KB all the way to 32MB! We will be employing this level of cache in situations such as when we need to evict something from our L1 cache and we need to write back to the next level of cache which will be our L2 cache. 



**Assumptions:**
We are not checking for data, only addresses.
The first time the processor reads data, it goes to exclusive state because no other processor has any shared data.
If a read miss occurs, the other L1 cache is checked and if it does not contain the data, main memory is accessed and writes to the cache. 
If all cache lines are filled, we use the LRU cache line and evict it for new data. 


This cache will also be employing a snooping protocol or a way to check other memory transactions to know what is occurring.Our outermost cache will be snooping. This also uses states to determine whether the data in the cache needs to be updated and if data is fine to be written to the block of memory. 



## Current State of the Project

By the end of the project, our model was able to provided the expected output for both the trace files provided for testing and for the demo. Only minor modifications were needed for the file handling for the demo trace file. 


## Post Project Notes




## Installation

<!-- Specify what the primary c file is -->
<!-- Potential reorganize repo and divert the other files into an archive folder -->



<!--
## Usage

Provide instructions and examples for use. Include screenshots as needed.

To add a screenshot, create an `assets/images` folder in your repository and upload your screenshot to it. Then, using the relative filepath, add it to your README using the following syntax:

    ```md
    ![alt text](assets/images/screenshot.png)
    ```

## Features

If your project has a lot of features, list them here.

## Tests

-->

## Credits

<!-- List your collaborators, if any, with links to their GitHub profiles. -->
- Kai Roy
- Nick Allmeyer
- Kamal Smith
- Jesus Zavala
- Daisy Perez

