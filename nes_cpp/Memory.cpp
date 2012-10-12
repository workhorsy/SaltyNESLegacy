

#include "nes_cpp.h"


	 Memory::Memory(NES* nes, int byteCount){
		this->nes = nes;
		mem = new vector<short>(byteCount);
		memLength = byteCount;
	}
	
	 Memory::~Memory() {
		nes = NULL;
		delete_n_null(mem);
	}
	
	 void Memory::stateLoad(ByteBuffer* buf){
		
		if(mem==NULL)mem=new vector<short>(memLength);
		buf->readByteArray(mem);
		
	}
	
	 void Memory::stateSave(ByteBuffer* buf){
		
		buf->putByteArray(mem);
		
	}
	
	 void Memory::reset(){
		for(size_t i=0; i<mem->size(); i++)
			(*mem)[i] = 0;
	}
	
	 int Memory::getMemSize(){
		return memLength;
	}
	
	 void Memory::write(int address, short value){
		(*mem)[address] = value;
	}
	
	 short Memory::load(int address){
		return (*mem)[address];
	}
	
	 void Memory::dump(string file){
		dump(file, 0, mem->size());
	}
	
	 void Memory::dump(string file, int offset, int length){
		
		char* ch = new char[length];
		for(int i=0; i<length; i++){
			ch[i] = (char)(*mem)[offset+i];
		}
		
		try{
	        ofstream writer(file.c_str(), ios::out|ios::binary);
			writer.write((char*)ch, length);
			writer.close();
			printf("Memory dumped to file \"%s\".\n", file.c_str());
			
		}catch(exception& ioe){
			printf("%s\n", "Memory dump to file: IO Error!");
		}
		
		
	}
	
	 void Memory::write(int address, vector<short>* array, int length){
	
		if(address+length > (int) mem->size())return;
		arraycopy_short(array, 0, mem, address, length);
		
	}
	
	 void Memory::write(int address, vector<short>* array, int arrayoffset, int length){
		
		if(address+length > (int) mem->size())return;
		arraycopy_short(array,arrayoffset,mem,address,length);
		
	}
	
	
