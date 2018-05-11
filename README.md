# GameAI
Game development project featuring four artificial intelligence challenges: Conway's Game of Life, Lunar Lander, Chess and Rainbow Road.
To change between challenges, edit the enum supplied in GameScreenManager initalization in Source.cpp:53.

# Artificial Life
The artificial life section of the project implements Conway’s Game of Life. It consists of a 2D grid of cells, with some alive and others dead, and a set of rules that govern the how they come alive and die. The implementation for this project starts with a 2D array of integers (int[x][y]) the size of the screen. On initialization, these cells are randomly set to alive or dead. Alive cells render as black and white cells render as dead. Every frame Conway’s rules are applied to the cells and their state is changed as necessary.

To apply these rules, a copy of the grid is made and then each cell looped over, crossing every column and each column’s rows. At each cell, the 9 cells surrounding the current cell are looped over and the number of surrounding cells that are alive is summed using the below algorithm:

~~~~
for (int iWidthOffset = -1; iWidthOffset < 2; iWidthOffset++)
for (int iHeightOffset = -1; iHeightOffset < 2; iHeightOffset++)
if (!(iWidthOffset == 0 && iHeightOffset == 0))
if (gridCopy[iWidth + iWidthOffset][iHeight + iHeightOffset])
surroundingAliveCells++; 
~~~~ 

Where iWidthOffset is the index of the column, iHeightOffset is the index of the row, gridCopy is the copy of the 2D array containing the cells and surroundingAliveCells is a simple count that increments when a surrounding cell is alive. On the third line there is a check in place that makes sure we don’t consider the current centre cell when looking for surrounding alive cells. Once we know the number of surrounding cells which are alive we can apply the rules to the centre cell. 

Rule 1: Any live cell with fewer than two live neighbors dies, as if caused by under-population. This rule is true if the cell is alive and surroundingAliveCells is less than two, setting the centre cell to dead. 

Rule 2: Any live cell with two or three live neighbors lives on to the next generation. This is true if the cell is alive and surroundingAliveCells is equal to 2 or 3, leaving the centre cell alive.

Rule 3: Any live cell with more than three live neighbors dies, as if by overcrowding. This is true if the cell is alive and surroundingAliveCells is greater than 3, setting the centre cell to dead.

Rule 4: Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction. This is true if the cell is dead and surroundingAliveCells is equal to 3, setting the centre cell to alive.

# Evolutionary Algorithms
The evolutionary algorithms section of the project implements a Lunar Lander game with a genetic algorithm. It consists of a number of landers which can move about in the air with the aim of landing on a small platform. If the lander hits anything else in the world or comes down on the platform too fast it will crash. The implementation for this project involves initializing landers with random ‘chromosomes’. A chromosome contains a sequence of flying instructions for the lander such as move left, move right, thrust upwards etc. Running a generation involves executing the sequence of flying instructions stored in each landers chromosomes. The objective is to evolve the chromosomes so that every lander successfully lands on the platform. This is achieved through 4 stages: fitness, selection, crossover and mutation. 

The fitness stage happens first, which involves taking a generations chromosomes and assigning them a fitness value each. Multiple calculations take place to come up with an overall score for the lander including: distance from the platform, rotation angle and time taken. The following algorithm is used to come up with those 3 values: 

~~~~
distanceFitness = kLunarScreenWidth - std::sqrt(x_diff * x_diff + y_diff * y_diff);
rotationFitness = 500 * (1 / (std::abs(rotAngle) + 1));
speedFitness = survivalTime / (speed + 1);
~~~~

Where kLunarScreenWidth is the total width of the screen, x_diff and y_diff are the x and y finishing distance of the lander from the platforms centre, rotAngle is the finishing rotation of the lander, survivalTime is the time the lander survived for before crashing/landing and speed is the finishing speed of the lander. The reason for multiplying rotationFitness by 500 is because I found distanceFitness and speedFitness to both end up around 400, but rotation angle was a value from 0 to 1 so it wouldn’t have affected the total fitness at all. All of these fitness values added together result in the landers fitness for that generation. One extra thing to note is that if the lander had successfully landed on the platform its fitness would be set to 10000 - survivalTime. This is because it isn’t possible for that lander to improve its fitness, only the time it takes to reach the platform.

The next stage, selection, is about finding the ‘fittest’ set of chromosomes. The technique used is a tournament style approach, where a number of landers battle to find the highest fitness value. When the highest fitness is found among the contenders, the set of chromosomes from the fittest lander is selected and stored in another variable, ready for the next stage. This process is repeated until we have the original number of lander’s set of fit chromosomes. This method does allow for duplicate sets of chromosomes to be selected but it isn’t a problem as two sets of fit chromosomes is better than one fit set and one bad set.

The next stage, crossover, makes use of the fit sets of chromosomes we selected previously and adds some variation. The way this is done is by swapping individual chromosomes from sets with other sets. Specifically, it is done in sequential pairs, with each set in the pair giving half its chromosomes to the other set.

Finally, the mutation stage adds another level of variation to the chromosomes if it is needed. It simply sets a random chromosome in each set to a random value. However there is a condition to doing this. There is a global variable called mMutationRate which is a value from 0 to 1000. Before the mutation happens on each set of chromosomes a random number is generation and if it is higher than the current mutation rate the mutation doesn’t happen. The purpose of this variable is to reduce the chances of mutating away a chromosome that may be key to a lander successfully landing. The mutation rate value is fluctuates depending on the number of successful landings in the previous generation. If there is low landings the mutation rate is set to very high in an attempt to randomly generate the chromosome that with fix the landers flight. If there is high landings the mutation rate is set to very low in an attempt to maintain the successfully landing lander’s chromosomes intact. From here the chromosomes are put back into the ‘evolved’ landers and the next generation takes place.

An alternative approach to using a genetic algorithm would be a memetic algorithm, which is an extension of the former. It’s main differences include individual chromosomes being able to improve in-generation. As opposed to genetic algorithms where all the improving is done at the end of a generation with crossovers and mutations. The reason a memetic algorithm would’ve been more effective is that if it can reduce the likelihood of premature convergence, where all the population shares the same chromosomes. This is because the population is updating individually before breeding takes place, varying the chromosomes further [1].

# Decision Theory
The decision theory section of the project implements a chess AI player with the purpose to play chess at an intelligent level. The AI uses the minimax algorithm with the alpha beta pruning approach in addition to positional score tables and total piece evaluations. 

At its most basic level, the minimax is used to find the best move for the AI whilst taking into account its opponent’s possible best moves. It essentially looks ahead a number of moves into the future, moving the game along one move at a time by finding its best possible scoring move and then finding its opponent’s best scoring move after that move. It then assumes the opponent will take his best possible move and then repeats the process a number of times. Each possible move at the beginning will lead the algorithm down a path of moves until a limit is reached. At the limit all paths return their score and whichever path scored the best, the first move of the path is the one the AI will take. 

This works with 3 functions: Maximize, Minimize and Board Scoring. The maximize and minimize functions are both recursive and almost identical. They both deal with finding a high scoring move for a player from any board state. Except that maximize is evaluated from the AI player’s point of view and minimize is evaluated from the opponent’s.

The maximize function is called first and seeks to find the best move for the AI player. It receives a Board object, which contains information where every piece is on the board and, as the function is recursive, the current depth is passed to the function to stop us from searching too deep. Current depth is simply the number of moves that the minimax algorithm has taken so far. As the number of moves needed to be checked increases exponentially, there will be performance issues if the maximum search depth is too deep. If the maximum depth is reached the board is scored (described in detail further on) and returned to the previous level.

If the depth we’re at isn’t too deep we can then fetch all the possible moves for the AI player and iterate over them. For each iteration, which is a possible move, we create a copy of the Board object passed to the function and perform the possible move in the current iteration on the board. With this Board object with the move on it, we pass it to the minimise function, as well as move down a depth. 

The minimise function is identical in every way to the maximise function other than it fetches all the possible moves for the opponent player. It gets the opponents moves from the new board, with the added move from the maximise function. And after the minimise iterates through its moves and adds them to the board, it calls the maximise function and moves down a depth. This process repeats until the maximum depth is reached.

Once the maximum depth is reached the board is scored and the score returned up a depth level to a parent maximise or minimise function, where the score value is stored as the best score. As the iterator does the next move it will return another score value. If the parent function we are currently in is a maximise function we are searching for the highest score. If the new score value is greater than the previous value, the new score value will become the best value. If we are in a minimise function we are doing the opposite and searching for the lowest score. Once the iterations have completed and the best scores have worked their way up to the starting depth, a best possible score will be revealed. The move that generated that score is the move that the AI player will take. 

When the maximum depth limit is reached and the board needs to get scored, a few factors contribute to the overall score of a board. Firstly positional score tables are used to give specific pieces a higher score depending on where they are currently on the board. For example, rooks are better used where they have long lines of sight, such as edges of the boards, and are much less useful in the centre of the board. There before if a rook was positioned on a edge they would get +50 but if they were in the centre of the board it would be -50. There are position tables designed for every piece. In addition to this there are total piece evaluations, where the number of pieces left on each team impacts your score, multiplied by a piece modifier depending on how value the piece is. 

One additional method of improving the minimax algorithm is through the alpha beta pruning approach. This approach is used to break out of a path early by determining that a better move already exists, making the algorithm 1.5 times faster. Alpha is the best value that the current maximise iterations can guarantee at that depth or above. Beta is the best value that the current minimise iterations can guarantee at that depth or above. 

In the maximise function, a beta value is passed into it. Beta is the highest value the parent minimise function can guarantee and is initialised to max integer at the start of the algorithm. Inside the maximise function alpha is initialised to negative max integer. Each iteration alpha becomes the highest score returned from the minimise function like before with base value:

~~~~
alpha = max(alpha, Minimise(newBoard, depth + 1, alpha));
~~~~

Where newBoard is the copy of the board with the current move added to it, depth is the current depth we are at in the algorithm and alpha is the current highest score returned from the minimise function. The next check is that if alpha is greater than beta, the highest value currently from the parent minimise function, return alpha as the best score for this depth. This is because in the parent minimise function we are searching for the lowest score possible. Beta is what the current lowest score is for the parent minimise function. The alpha we just calculated, the highest value so far in the iteration and the value that will be returned to the parent minimise function, is higher the lowest value in the parent minimise function, therefore the lowest score can’t possibly be found down this path. In the minimise function it is very similar to the maximise function, except with some values and signs switched around and with the return condition being if beta is less than alpha.

An alternative to the minimax algorithm would be the expectimax algorithm. The difference being an additional node the path can go down other than maximise and minimize: chance. The chance node is interleaved with the maximize and minimize node and work by instead of taking the max or min values of their children, it takes a weighted average, with the weight being the probability that child is selected. The point of the chance node is to give the AI a decision when the current situation doesn’t present any strategic advantages and or disadvantages, simply a ‘move by nature’ move for the AI [2].

# Artificial Neural Networks
The artificial neural networks section of the project implements the rainbow islands 2D platforming game with the player controlled by a evolutionary neural network. The neural network takes multiple inputs from the game world and turns them into outputs, which in turn move the player. In the background there is a genetic algorithm working, back propagating the sets of weights to evolve them to find a solution. 

The first part of the solution is the neural network. The neural network consists of a number of NeuronLayer objects which consist of Neuron objects. A Neuron holds a set of weights which are just random decimal values between 0 and 1 and a NeuronLayer just holds a set of Neurons. A NeuronLayer is what processes inputs and turns them into outputs and will be described in more detail later.

With the neural network being the brain for our character, we have a CharacterAI object as the body, which responds to inputs like move left or right. Every frame we need to feed the neural network information about the body of the character, which are defined as inputs. Inputs can be any bit of data about the world during that frame and can be any quantity. 

In this implementation there are 41 inputs to the neural network. The first and second inputs are distances to the closest fruit and closest enemy, calculated as a percentage of the screen height scaled between 0 and 1. Another input is the player position from the top as a percentage scaled between 0 and 1. The next two inputs represent the horizontal direction needed to travel to reach the closest fruit. If it's to left the two inputs are 0, 1 or if it’s to the right the two inputs are 1, 0. The next input is 1 if the player is currently on a rainbow or 0 if they are not.

The other 35 inputs come from the immediate surrounding tiles of the player with any tiles with an enemy or rainbow or if the tile is outside the bounds returning a 1. These 45 inputs are fed into the neural network which then loops through each NeuronLayer processing each Neuron and summing up the inputs * weights and calculating each neuron’s activation by putting the total through a sigmoid function. This returns 4 outputs to the character: left, right, jump and spawn rainbow. The outputs are decimals and so a value over 0.8 is needed to fully trigger the output in the game.

The second part of the solution is the genetic algorithm. This is very similar to evolutionary algorithm described previously in that it calculates fitness and performs a selection, crossover and mutation. Instead using landers with chromosomes we are using Genomes, which contain a set of weights and a fitness value. In this solution a weighted run is when the AI player attempts to reach the top of the level, with the neural network loaded with a set of weights, which ends in a fitness score. A generation is when 10 weighted runs have been attempted. Once a generation has finished, the fittest few run’s weights are selected as the elite weights and distributed to the rest of the weights, following the selection, crossover and mutation procedures as described before. Then the new, fitter weights are loaded run by run into the neural network to perform their weighted runs.

References
[1] Moscato, P., Cotta, C. A Gentle Introduction to Memetic Algorithms. (1989).
[2] Zettlemoyer, L. Expectimax Search. (2008).
[3] Buckland, M. AI Techniques for Game Programming. (2002).
