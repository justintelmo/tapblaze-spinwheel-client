# TapBlaze Coding Test

By Justin Telmo

## Description

A Cocos2D project that simulates a wheel being spun, with the user being shown what they landed on. Calls are simulated on the server, so the client has no authority
when it comes to the result of the spin. The backend is a simple Laravel PHP one, but thanks to the Laravel framework could be scaled to accommodate more features.


## Time Taken

Overall, this took me just over 15 hours of work, most of which was debugging and figuring out how to get an Android emulator to work on my machine.

## Technologies Used

C++

- Cocos2DX

PHP

-   Laravel

Javascript

-   Bootstrap
-   jQuery

## Unit Testing

I have included a sample of the unit testing I did for this, which can be found in proj.win32/testoutput.txt

## Server Results

I have a live page on Heroku right now that both tracks the most recent spins and shows the current (live) weights. Weights may be modified at will and reflect in real time on the client,
without a need to restart.
https://justintelmo-tapblaze-spinwheel.herokuapp.com/weights

This was done fairly quickly so that I could focus more heavily on the C++ aspect of the project.

## Scaling Ideas

If this game were to suddenly reach 1m DAU, there would be a few steps I would take to ensure that the game could handle that many potential requests. 

First of all, I would introdce a couple redundant caching layers in the forms of Redis and Couchbase. These are two tools I have used before as caching solutions between 
a PHP API and a MySQL host. Caching greatly reduces the load on the MySQL server, and reduces the response time of each request. 

To get into more specifics, I would create a Redis List that would appropriately segment each portion of total user spins. This list would have a fairly low TTL, but the point of having
it would be to accumulate user spins until it is appropriate to flush the Redis list into a Couchbase key. This batch job would run semi-frequently.

The next step I would take is to containerize our server code base so that it can be properly deployed across multiple instances in whatever cloud solution is available.
I am used to Docker & AWS, but I am aware that there are other cloud computing solutions out there (Azure, GCP). There would need to be a cost conversation, as these services
usually cost more as scale increases.

In terms of design, adopting microservice architecture patterns into the codebase would help keep things light, allowing for faster iteration as the number of features scale
alongside the user base. 

Lastly, setting up proper logging, monitoring, and testing goes a fairly long way in terms of allowing a game to scale. Tools like Grafana and New Relic allow engineers 
to monitor the health of the game 24/7, ensuring that if downtime does occur, it is mitigated. 


