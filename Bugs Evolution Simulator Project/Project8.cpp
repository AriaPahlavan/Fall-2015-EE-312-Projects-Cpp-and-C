//Project 8
//Aria Pahlavan - ap44342
//EE 312 - Fall 2015

#include "Bugs.h"
#include <stdlib.h>
#include <assert.h>

int time_step = 0;
int average_age = 0;
int average_generation = 0;
int percent_straight;
int percent_right;
int percent_left;
int percent_back;
int total_left = 0;
int total_right = 0;
int total_straight = 0;
int total_back = 0;

/* DONE: you don't need to use this function in your solution
* however this function is used by createWorld to generate random bugs */
Bug createBug(int x, int y) {
	Bug b;
	b.age = 0;
	b.generation = 0;
	b.x = x;
	b.y = y;
	b.dir = rand() % 8;
	b.health = START_HEALTH;
	for (int k = 0; k < 8; k += 1) {
		if (DISTRIBUTED_GENES) {
			b.genes[k] = GENE_TOTAL / 8;
		}
		else {
			if (k == 0) {
				b.genes[k] = GENE_TOTAL;
			}
			else {
				b.genes[k] = 0;
			}
		}
	}
	total_straight += b.genes[0];
	total_right += b.genes[1] + b.genes[2] + b.genes[3];
	total_left += b.genes[5] + b.genes[6] + b.genes[7];
	total_back += b.genes[4];
	return b;
}

/* DONE: */
void createWorld(void) {
	/* empty the world */
	for (int i = 0; i < WORLD_SIZE; i += 1) {
		for (int j = 0; j < WORLD_SIZE; j += 1) {
			world[i][j] = EMPTY;
		}
	}

	/* create food */
	for (int k = 0; k < INITIAL_FOOD; k += 1) {
		int x = rand() % WORLD_SIZE;
		int y = rand() % WORLD_SIZE;
		world[x][y] = FOOD;
	}

	/* create bugs */
	while (bug_list.size() < INITIAL_BUGS) {
		int x = rand() % WORLD_SIZE;
		int y = rand() % WORLD_SIZE;
		if (world[x][y] == EMPTY) {
			world[x][y] = bug_list.size();
			bug_list.push_back(createBug(x, y));
		}
	}
}

/*
* DONE: keep in mind that bugs that "fall off" the edge of the world
* automatically reappear on the opposite edge (i.e, the world is round,
* well, if you really want to get topographical, our bugs live on a donut.
*/
int wrap(int y) {
	if (y < 0) { return WORLD_SIZE - 1; }
	else if (y == WORLD_SIZE) { return 0; }
	else { return y; }
}

/* DONE: you can use this function if you want, it's not required that you use it */
int newX(int x, int dir) {
	switch (dir) {
	case 0: // move up
	case 1:
	case 7:
		return wrap(x - 1);
		break; // not reached
	case 3: // move down
	case 4:
	case 5:
		return wrap(x + 1);
		break; // not reached
	case 2: // move left or right
	case 6:
		return x;
	}
	/* NOT REACHED */
	return 0;
}


/* DONE: you can use this function if you want, it's not required that you use it */
int newY(int y, int dir) {
	switch (dir) {
	case 1:
	case 2:
	case 3:
		return wrap(y + 1);
		break; // not reached
	case 5:
	case 6:
	case 7:
		return wrap(y - 1);
		break; // not reached
	case 0:
	case 4:
		return y;
	}
	/* NOT REACHED */
	return 0;
}


/* DONE:
* Add NUM_FOOD food objects in random locations
* if you drop food on a bug, be sure to feed the bug
*/
void addFood(void) {
	for (int food = 0; food < NUM_FOOD; food += 1) {
		int x = rand() % WORLD_SIZE;
		int y = rand() % WORLD_SIZE;
		switch (world[x][y]) {
		case EMPTY: // empty, add the food
			world[x][y] = FOOD;
			break;
		case FOOD: // already has food, do nothing
			break;
		default: // bug, feed the bug
			bug_list[world[x][y]].health += EAT_HEALTH;
		}
	}

}

/*Finds the direction which the bug chose depending on its genes*/
int directionFinder(const Bug b, int rand) {
	/*
	for (int i = 0; i < 8; i++) {
		int weight = b.genes[i];
		if (weight > rand) {
			return ((b.dir + i) % 8);
		}
		else rand -= weight;
	}*/
	 
	 int sum = 0;
	 int i = 0, c = 7
	 {sum += b.genes[i];
	 if (sum >= rand)	return i;
	 }
	 while(c > 0){
		 
		 if (c%2 == 1) {
			 i += c;
			 sum += b.genes[i];
			 if (sum >= rand)	return i;
			 c -= 1;
		 }
		 else {
			 i -= c;
			 sum += b.genes[i];
			 if (sum >= rand)	return i;
			 c -= 1;
		 }

	 }
	 //NOT REACHED
	return -1;
}



/* TODO... finish this function as described below
* move bugs
* if they move onto food, feed them
* if they move onto another bug, no big deal.  Two bugs
* can be in the same square.  Let's say bug#2 and bug #6 are in the same
* square.  Well, both bugs are still in the bug_list, so they
* will still be able to move next time step.
* Since the world[][] array can only hold either a 2 or a 6, we'll just
* put one of them there.  The easiest thing to do is just put which
* ever bug moves into the square first into the square.  When the next
* bug moves into the square, update his x and y (in bug_list) so that
* he is in the square, leave the other bug alone (i.e., it's also in
* the same square) and then set world[x][y] = 6 (the bug # of the second
* bug).  No harm done.  The graphics will draw only the second bug
* but who cares.  The next time step they'll move into different squares
* (probably) and no one will notice.
* NOTE: only the first bug to move into the square gets to eat the food
* there.
*/
void moveBugs(void) {
	int total_age = 0;
	int total_gen = 0;

	/* update each bug in turn (but don't kill them) */
	for (int k = 0; k < bug_list.size(); k += 1) {
		Bug b = bug_list[k];
		/* TODO: Move bug k one step foward in its current direction */
		int prevDir = b.dir;
		int xNew = newX(b.x, b.dir);
		int yNew = newY(b.y, b.dir);
		if (xNew == b.x && yNew == b.y) abort();
		/* TODO: see if we stepped on food! */
		if (world[xNew][yNew] == FOOD) {
			b.health += EAT_HEALTH;
		}
		/* TODO: update the world: NOTE: we may put this bug on top of another one
		* but that is OK, as long as the other one is in the bug list */
		world[b.x][b.y] = EMPTY;
		b.x = xNew;
		b.y = yNew;
		world[b.x][b.y] = k;
		/* TODO: update this bug's health due to movement cost */
		b.health -= MOVE_HEALTH;
		/* TODO: pick a new direction for the bug, based on its genes */
		int randNum = rand() % GENE_TOTAL;
		b.dir = directionFinder(b, randNum);
		/* DONE: update this bug's age for statistics */
		b.age += 1;
		total_age += b.age;
		total_gen += b.generation;
	}

	/* DONE: some statistics... */
	average_age = total_age / bug_list.size();
	average_generation = total_gen / bug_list.size();
}

//this function only updates the stat variables
void updateStats(int i) {
	total_straight -= bug_list[i].genes[0];
	total_right -= (bug_list[i].genes[1] + bug_list[i].genes[2] + bug_list[i].genes[3]);
	total_left -= (bug_list[i].genes[5] + bug_list[i].genes[6] + bug_list[i].genes[7]);
	total_back -= (bug_list[i].genes[4]);
}

void removeBug(int i) {
	//update the world  **** temp is the last one
	world[bug_list[i].x][bug_list[i].y] = EMPTY;
	updateStats(i);
	//if we're dealing with the last one just delet and return
	if (i != bug_list.size() - 1) {
		//swap
		Bug temp = bug_list[bug_list.size() - 1];
		world[temp.x][temp.y] = i;
		bug_list[bug_list.size() - 1] = bug_list[i];
		bug_list[i] = temp;


		//pop_back last
		bug_list.pop_back();
	}
	else {
		//pop_back last
		bug_list.pop_back();
	}
}
/* TODO: write this function
* look through the bug_list for dead bugs (bugs with zero
* health) and kill them
*
* To kill a bug, mark it's position in the world as EMPTY
* then remove it from the bug_list.
*
* To remove a bug from the bug_list, use the following simple
* trick.  Swap the bug with the last bug in the list, then
* do "pop_back" on the bug_list vector.
* Hey, since we're going to do pop_back anyway, we don't really
* need to swap with the last bug.  We just need to copy the
* last bug into our position.
*
* WATCH OUT: there's a trap.  Be careful to account for the possibility
* that the "last bug" is also dead.  Don't miss out on killing it
* because you moved that bug forward in the bug_list
*
* Be sure to update the global statistics (total_straight, total_right, total_left and total_back)
* for each bug that you kill.
*/

void killDeadBugs(void) {
	//Bug temp;
	int i = 0;
	while (i < bug_list.size()) {
		if (bug_list[i].health == 0) {
			//1)remove the bug
			removeBug(i);
			//2)check again to see if still need to be cleared
			if (i < bug_list.size()) {
				if (bug_list[i].health == 0) i -= 1;
			}
		}

		i++;
	}
}

/* TODO: Finish this function
* Look at all the bugs in the bug_list.  For each bug
* that has enough health to reproduce, reproduce a new bug.
* Put the new bug in the same square and have it head in the
* same direction.
*
* Give both the new bug and the old bug 1/2 the energy of the
* old bug.
*
* Mutate the genes of the new bug as follows.  Choose two genes
* at random.  Increment one of those genes (by one) and decrement
* the other gene (by one).
* YOU MUST ensure
* 1. that no genes ever get smaller than zero
* 2. the total of the eight genes is always GENE_TOTAL
*
* Be sure to put the new bug in the bug_list (no need to update
* the world, since you have two bugs in the same square anyway).
*/
void reproduceBugs(void) {
	int num_bugs = bug_list.size();
	for (int k = 0; k < num_bugs; k += 1) {
		if (bug_list[k].health >= REPRODUCE_HEALTH) {
			/* TODO: make "b" a copy of ourself, but with a generation count 1 greater than our own */
			Bug child = bug_list[k];
			child.age = 0;
			child.generation += 1;
			/* TODO: reduce our health by half and assign b half our health */
			bug_list[k].health /= 2;
			child.health /= 2;
			/* TODO: mutate Bug b's genes based on the instructions in the handout */
			int random = rand() % 8;
			while (child.genes[random] < 1) { random = rand() % 8; }
			child.genes[random] -= 1;
			random = rand() % 8;
			child.genes[random] += 1;
			/* DONE: update global statistics based on the new genes in b */
			total_straight += child.genes[0];
			total_right += child.genes[1] + child.genes[2] + child.genes[3];
			total_left += child.genes[5] + child.genes[6] + child.genes[7];
			total_back += child.genes[4];

			/* DONE: add b to the global bug list */
			bug_list.push_back(child);
		}
	}
}

void timeStep() {
	time_step += 1;
	addFood();
	moveBugs();
	killDeadBugs();
	reproduceBugs();
	percent_straight = 100 * total_straight / bug_list.size() / GENE_TOTAL;
	percent_left = 100 * total_left / bug_list.size() / GENE_TOTAL;
	percent_right = 100 * total_right / bug_list.size() / GENE_TOTAL;
	percent_back = 100 * total_back / bug_list.size() / GENE_TOTAL;
}
