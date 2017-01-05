# computer_architecture-a-simple-branch-predictor-
Your goal is write code to evaluate the performance of a 2-bit saturating counter based branch predictor on this trace.
Branch Predictor Architecture
Your design must consist of 2m 2-bit saturating counters indexed using m LSBs of each branch instruction. 
Each 2-bit saturating predictor starts in the 11 (Predict Taken with High Confidence) state and is updated as 
per the finite state machine discussed in Lecture 11. The value of m is specified in a config file config.txt. 
