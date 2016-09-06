void liberaStruct(DLX* machPtr)
int i;
// ************************ Memory
//	machPtr->memPtr = (MemWord *)   calloc(1, (unsigned) (sizeof(MemWord) * machPtr->numWords));
	free (machPtr->memPtr);
//	      machPtr->memScratch = (char *) malloc (machPtr->numChars);
	free(machPtr->memScratch);

// *********************** Symbol table
//	Hash_InitTable(&machPtr->symbols, 0, HASH_STRING_KEYS);
	Hash_DeleteTable(&machPtr->symbols); //es posible que haya que hacer una función para liberar la tabla hash
	free(machPtr->symbols);
// ************************ BTB
//	  machPtr->branchBuffer = (BranchInfo *) calloc(1, (unsigned) (sizeof(BranchInfo) * machPtr->bb_size));
	free(machPtr->branchBuffer);

// ************************ Instruction queue
//	    machPtr->insQueue->table = (InstructionElement *)calloc(1,(unsigned)(sizeof(InstructionElement) *(machPtr->insQueue->numEntries)));
	free(machPtr->insQueue->table);

//********************* integer window
//elementos de la ventan de instrucciones que deben ser liberados antes que la ventana

for (i=0,i<machPtr->iWindow->numEntries;i++) {
	    free(machPtr->iWindow->freeList->firstOperand);
		free(machPtr->iWindow->freeList->SecondOperand);
}
//    machPtr->iWindow->table = (WindowElement *)calloc(1,(unsigned)(sizeof(WindowElement) * (machPtr->iWindow->numEntries)));
	free(machPtr->iWindow->table);

//*********************** fp window
// ventana de flotantes;se debe hacer igual que la de enteros

for (i=0,i<machPtr->fpWindow->numEntries;i++) {
	    free(machPtr->fpWindow->freeList->firstOperand);
		free(machPtr->fpWindow->freeList->SecondOperand);
}

//   machPtr->fpWindow->table = (WindowElement *)	calloc(1,(unsigned)(sizeof(WindowElement) * (machPtr->fpWindow->numEntries)));

	free(machPtr->fpWindow->table);

//*********************** reorder buffer
// machPtr->iReorderBuffer->element = (ReorderElement *) calloc(1, (unsigned) (sizeof(ReorderElement) * (machPtr->iReorderBuffer->numEntries + 1)));
	free( machPtr->iReorderBuffer->element);

// machPtr->fpReorderBuffer->element =  (ReorderElement *) calloc(1, (unsigned) (sizeof(ReorderElement) * (machPtr->fpReorderBuffer->numEntries + 1)));
	free( machPtr->fpReorderBuffer->element);
//************************ Store buffer
// liberamos los elementos del buffer store

for (i=0,i<machPtr->storeBuffer->numEntries;i++) {
	    free(machPtr->storeBuffer->freeList->dataToStore);
}

//    machPtr->storeBuffer->table = (DataOp *) calloc (1, (unsigned)(sizeof(DataOp) * (machPtr->storeBuffer->numEntries)));
	free(machPtr->storeBuffer->table);

//********************* load buffer

//    machPtr->loadBuffer->freeList = (DataOp *)calloc (1, (unsigned)(sizeof(DataOp) * (machPtr->loadBuffer->numEntries)));
	free(machPtr->loadBuffer->freeList);
//********************* Statistics
//    machPtr->counts = (Statistics *) calloc(1,sizeof(Statistics));
	free(machPtr->counts);


}