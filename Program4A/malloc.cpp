#include <unistd.h> // sbrk( )
#include <limits.h> // LONG_MAX

using namespace std;

static bool initialized = false;
static void *heap_top; // the beginning of the heap space
static void *heap_end; // the current boundary of the heap space, obtained from sbrk( 0 )

class MCB { // memory control block
public:
  int available; // true(1): this memory partition is available, false(0) unavailalbe.
  int size;      // MCB size + the user data size
};

void free_( void *dealloc_space ) {
  MCB *mcb;

  mcb = (MCB *)((unsigned long long int)dealloc_space - sizeof(MCB));
  mcb->available = 1;
  return;
}

void *malloc_f( long size ) {
  struct MCB *cur_mcb;          // current MCB
  void *new_space = NULL; // this is a pointer to a new memory space allocated for a user

  if( !initialized )   {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk( 0 );
    heap_top = heap_end;
    initialized = true;
  }

  // append an MCB in front of a requested memroy space
  size = size + sizeof( MCB );

  for(void *cur = heap_top; cur < heap_end; cur = (void *)((unsigned long long int)cur + cur_mcb->size)) {
    cur_mcb = (MCB *)cur;

    if((cur_mcb->available == 1) && (cur_mcb->size >= size)) {
      cur_mcb->available = 0;

      new_space = cur_mcb;

      break;
    }
  }

  // no space found yet
  if (new_space == NULL) {
    sbrk(size);
    
    new_space = heap_end;

    heap_end = (void *)((unsigned long long int)heap_end + size);
   
    cur_mcb = (MCB *)new_space;
    cur_mcb->available = 0;
    cur_mcb->size = size;
  }

  // new space is after new MCB
  return (void *)( ( long long int )new_space + sizeof( MCB ) );
}

void *malloc_b( long size ) {
  struct MCB *cur_mcb;          // current MCB
  void *new_space = NULL; // this is a pointer to a new memory space allocated for a user

  if( !initialized )   {
    // find the end of heap memory, upon an initialization
    heap_end = sbrk( 0 );
    heap_top = heap_end;
    initialized = true;
  }

  // append an MCB in front of a requested memroy space
  size = size + sizeof( MCB );

  MCB *bestMCB = NULL;
  long minDiff = LONG_MAX;
                    
  for(void *cur = heap_top; cur < heap_end; cur =(void *)((unsigned long long int)cur + cur_mcb->size)) {

    cur_mcb = (MCB *)cur;

    if((cur_mcb->available == 1) && (cur_mcb->size >= size)) {
        
        long diff = cur_mcb->size - size;
        
        if(diff < minDiff) {
          
          minDiff = diff;
          bestMCB = cur_mcb;
        }
    }
  }
  
  if(bestMCB != NULL) {
    
    bestMCB->available = 0;
    new_space = bestMCB;
  }

  // no space found yet
  if ( new_space == NULL ) {
    sbrk(size);

    new_space = heap_end;

    heap_end = (void *)((unsigned long long int)heap_end + size);

    cur_mcb = (MCB *)new_space;
    cur_mcb->available = 0;
    cur_mcb->size = size;
  }

  // new space is after new MCB
  return (void *)( ( long long int )new_space + sizeof( MCB ) );
}
