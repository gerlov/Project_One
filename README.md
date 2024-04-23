# Rules
This can update as we go along

Do not Push straight to Master. 
Do not Edit straight to Master. 

Always, and always 
- Create your own branch based of Master and edit there
- push to your OWN branch.
- If you want to push to MAIN branch you need a SECOND person approval
-   Create a pull request
-   Use the "Review - Request" and ask someone to review it
-   If both OK's the code and any merge conflict is fixed. 
-   You are both now responsible for the merge.
-   And the MOST RECENT code from main merged together with your code works on YOUR test branch
-   In case of conflicts, contact the person the conflict arises with and solve the merge.
## How to compile
Run `make`\* to build both client and server. `make clean` to clean both the server and client.
To run the server: `make runserver`. To run a client `make runclient`. Each client needs its own terminal window if the server is run local.

When both the server and a client is running. Press space on the client, then a green box should pop up. Then press Space on the server window, to start the game. This will be replaced by menu later.


\* on windows, replace all `make` with `mingw32-make.exe`.

## You are the best
