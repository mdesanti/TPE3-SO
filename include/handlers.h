#ifndef HANDLERS_H_
#define HANDLERS_H_

/** Handlers for system calls **/

void handleCreateProcess(params * in);

void handleFormat();

void handleBlockProcess(params * p);

void handleGetPid(params * p);

void handleWakeProcess(params * p);

void handleWaitProcess(params * p);

void handleYield(params * p);

void handleKill(params * p);

void handleGetProcessName(params * p);

void handlePasswordMode();

void handleCannonicalMode();

void last100Handler(params * p);

void sleepPHandler(params * p);

void clearScreenHandler(params * p);

void handleDownSem(params * p);

void handleUpSem(params * p);

void handleGetSem(params * p);

#endif
