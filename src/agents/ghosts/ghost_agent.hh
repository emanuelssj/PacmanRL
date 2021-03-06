#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "../../state/direction.hh"
#include "../agent.hh"
#include "../../utils.hh"
#include "../../state/position.hh"
#include "../../pathfinding/pathfinding.hh"
#include <cmath>

class Ghost_Agent: public Agent {

public:

    virtual ~Ghost_Agent() {}
    //lunafreya
    int intelligenceGhost;
    Ghost_Agent(int intelligence) {
        intelligenceGhost = intelligence;
    }
    //lunafreya
    inline Direction take_action(const State& s, uint ghost_id) {
        const Ghost_State& ghost = s.ghosts[ghost_id];
        //int randomValue = rand() % 100 + 1;
        if (ghost.pos != ghost.prev) {
            if (s.is_scared(ghost)) {
                return PathMagic::try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos));
            }
            else {
                Direction d;
                //cout << s.pacman.pos.i << " - " << s.pacman.pos.j << endl;
                switch(ghost.behaviour) {
                    case SCATTER:
                        d = PathMagic::from_to(ghost.pos, ghost.scatter_pos); break;
                    case CHASE:
                        switch(ghost.typeGhost) {
                            case 0: d = PathMagic::from_to(ghost.pos, s.pacman.pos); break; // Blinky- only chase
                            case 1: if(clydeAction(s, ghost_id)) {
                                        d = PathMagic::from_to(ghost.pos, s.pacman.pos); break;
                                    }
                                    d = PathMagic::from_to(ghost.pos, pinkyAction(s, 4, ghost_id)); break; // Pinky - 4 pos in front of pacman
                            case 2: if(clydeAction(s, ghost_id)){
                                        d = PathMagic::from_to(ghost.pos, s.pacman.pos); break;
                                    } else if(inkyAction(s) > 0) {// Inky - if blinky on chase ((blinky distance of pacman) * 2) in front of pacman limit of 10 pos else run away
                                        d = PathMagic::from_to(ghost.pos, pinkyAction(s, inkyAction(s), ghost_id)); break;
                                    }
                                    d = PathMagic::try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos)); break;
                            case 3: if(clydeAction(s, ghost_id)) {// run from pacman when close <= 4 positions
                                       d = PathMagic::try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos)); break;
                                    }
                                    d = PathMagic::from_to(ghost.pos, s.pacman.pos); break;
                            default: ensure(false, "Invalid ghost behaviour enum");
                        }
                    
                }

                //ensure(pr.found, "Path not found!");
                if (d == Direction::STAY) _debug("No movement for ghost #%d - ghost_id #%d!", ghost.typeGhost, ghost_id);
                return d;
            }
        }
        else return ghost.dir;
    }

    inline Position pinkyAction(const State& s, int maxPosC, int ghost_id) {
        Position pacmanPosMatrix;
        Position finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
        int lastMov = 0;
        bool posMov = false;
        int maxPos = maxPosC;
        // 1 = RIGHT
        // 2 = UP
        // 3 = DOWN
        // 4 = LEFT
       if(s.pacman.dir == Direction::RIGHT) {
            for(int k = 0; k < maxPos; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            //Pinky position == Dest position
            if(finalPos.i == s.ghosts[ghost_id].pos.i && finalPos.j == s.ghosts[ghost_id].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }
            return finalPos;
        }
        if(s.pacman.dir == Direction::UP) {
            for(int k = 0; k < maxPos; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            //Pinky position == Dest position
            if(finalPos.i == s.ghosts[ghost_id].pos.i && finalPos.j == s.ghosts[ghost_id].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }
            return finalPos;
        }
        if(s.pacman.dir == Direction::DOWN) {
            for(int k = 0; k < maxPos; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            //Pinky position == Dest position
            if(finalPos.i == s.ghosts[ghost_id].pos.i && finalPos.j == s.ghosts[ghost_id].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }
            return finalPos;
        }
        if(s.pacman.dir == Direction::LEFT) {
            for(int k = 0; k < maxPos; k++) {
                posMov = false;
                pacmanPosMatrix = Position(finalPos.i, finalPos.j - 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 1 && posMov == false) {
                    lastMov = 2;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i + 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 3 && posMov == false) {
                    lastMov = 4;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i - 1, finalPos.j);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 4 && posMov == false) {
                    lastMov = 3;
                    posMov = true;
                }
                pacmanPosMatrix = Position(finalPos.i, finalPos.j + 1);
                if(pacmanPosMatrix.i > 0 && pacmanPosMatrix.j > 0 && pacmanPosMatrix.i < 29 && pacmanPosMatrix.j < 27 && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::WALL && s.maze[pacmanPosMatrix.i][pacmanPosMatrix.j] != State::SPAWN_AREA && lastMov != 2 && posMov == false) {
                    lastMov = 1;
                    posMov = true;
                }
                if(lastMov == 1)
                    finalPos = Position(finalPos.i, finalPos.j + 1);
                else if(lastMov == 2)
                    finalPos = Position(finalPos.i, finalPos.j - 1);
                else if(lastMov == 3)
                    finalPos = Position(finalPos.i - 1, finalPos.j);
                else
                    finalPos = Position(finalPos.i + 1, finalPos.j);
            }
            //Pinky position == Dest position
            if(finalPos.i == s.ghosts[ghost_id].pos.i && finalPos.j == s.ghosts[ghost_id].pos.j) {
                finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
            }       
            return finalPos;
        }
        if(finalPos.i == s.ghosts[ghost_id].pos.i && finalPos.j == s.ghosts[ghost_id].pos.j) {
            finalPos = Position(s.pacman.pos.i, s.pacman.pos.j);
        }
        return finalPos;
    }
        
    inline int inkyAction(const State& s) {
        int i, j;
        int limit = 10;
        (s.pacman.pos.i > s.ghosts[0].pos.i) ? i = s.pacman.pos.i - s.ghosts[0].pos.i : i = s.ghosts[0].pos.i - s.pacman.pos.i; // blinky pos and pacman pos
        (s.pacman.pos.j > s.ghosts[0].pos.j) ? j = s.pacman.pos.j - s.ghosts[0].pos.j : j = s.ghosts[0].pos.j - s.pacman.pos.j;
        if(s.ghosts[0].behaviour == Ghost_Behaviour::CHASE) {
            return ((i + j) * 2) > limit ? limit : ((i + j) * 2);
        }
        return 0;
    }

    inline bool clydeAction(const State& s, int ghostIndex) {
        int i, j, k;
        (s.pacman.pos.i > s.ghosts[ghostIndex].pos.i) ? i = s.pacman.pos.i - s.ghosts[ghostIndex].pos.i : i = s.ghosts[ghostIndex].pos.i - s.pacman.pos.i; // clyde pos and pacman pos
        (s.pacman.pos.j > s.ghosts[ghostIndex].pos.j) ? j = s.pacman.pos.j - s.ghosts[ghostIndex].pos.j : j = s.ghosts[ghostIndex].pos.j - s.pacman.pos.j;
        k = i + j;
        if(k <= 4)
            return true; // temporary
        return false;
    }
};

#endif
