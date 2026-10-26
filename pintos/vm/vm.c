/* vm.c: Generic interface for virtual memory objects. */

#include "threads/malloc.h"
#include "vm/vm.h"
#include "vm/inspect.h"
#include "lib/kernel/hash.h"
#include "filesys/file.h"
#include "vm/uninit.h"
#include "filesys/page_cache.h"
/* Initializes the virtual memory subsystem by invoking each subsystem's
 * intialize codes. */
void
vm_init (void) {
	vm_anon_init ();
	vm_file_init ();
#ifdef EFILESYS  /* For project 4 */
	pagecache_init ();
#endif
	register_inspect_intr ();
	/* DO NOT MODIFY UPPER LINES. */
	/* TODO: Your code goes here. */
}

/* Get the type of the page. This function is useful if you want to know the
 * type of the page after it will be initialized.
 * This function is fully implemented now. */
enum vm_type
page_get_type (struct page *page) {
	int ty = VM_TYPE (page->operations->type);
	switch (ty) {
		case VM_UNINIT:
			return VM_TYPE (page->uninit.type);
		default:
			return ty;
	}
}

/* Helpers */
static struct frame *vm_get_victim (void);
static bool vm_do_claim_page (struct page *page);
static struct frame *vm_evict_frame (void);

/* 초기화자를 사용하여 대기 중인 페이지 객체를 생성합니다. 생성하고 싶다면
 * 페이지를 직접 생성하지 말고 이 함수를 통해 생성하세요
 * `vm_alloc_page`.
 * spt -> uninit 으로 
 * 
 * 
 * */
bool
vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
		vm_initializer *init, void *aux) {

	ASSERT (VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current ()->spt;
	/*
	page_initializer
	-> 해당 페이지를 다른 페이지로 바꿔주는 애

	vm_initializer 
	-> 페이지를 할당하고 그에 대한 4kb 물리적 프레임 이쪽에다가 데이터를 채우는 애
	*/
	/* Check wheter the upage is already occupied or not. */
	if (spt_find_page (spt, upage) == NULL) {
		/* 할 일: 페이지 생성, VM 타입에 따라 초기화값 가져오기,
		 * TODO: 그 다음 uninit_new를 호출하여 "uninit" 페이지 구조체를 생성합니다. 너
		 * TODO: uninit_new 호출 후 필드를 수정해야 합니다. */
		/* 할 일: 페이지를 spt에 삽입한다. */
		struct page* new_page;
		if (type == VM_ANON){ // 이 부분 임시코드라 수정 필요 -> 모든 상황마다 uninit_new를 해야하는 지는 모르겠음
			uninit_new(new_page, upage, init, VM_ANON , aux, anon_initializer); 
		} 
		else if (type == VM_FILE){
			uninit_new(new_page, upage,  init, VM_FILE, aux, file_backed_initializer);
		}
		else if (type == VM_PAGE_CACHE){
			uninit_new(new_page, upage,  init, VM_PAGE_CACHE, aux, page_cache_initializer);
		}
		
		spt_insert_page(spt, new_page);
		return true;
	}
err:
	return false;
}
// load seg -> aux 정보 자체를 구조체로 넣어줘야 함
// 이 정보가 load_segment의 지역변수로 선언 시에 호출이 끝나면 날라가기 때문에 malloc을 통하여 전역 힙 공간에 선언해주고 free를 어디서 할 지 염두해둬야 함

/* Find VA from spt and return page. On error, return NULL. */
struct page *
spt_find_page (struct supplemental_page_table *spt UNUSED, void *va UNUSED) {
	struct page *page = NULL;
	/* TODO: Fill this function. */
	// spt 전체 리스트를 순회
	// va를 통하여 해당 값과 맞는 값을 찾으면 page 타입으로 반환
	// 만약 error 발생 하면 null을 리턴

	// 여기 반복문 -> 이 부분 내장되어있는 함수가 있다고 함
	for (struct hash_elem *start; true/*해시여서 무조건 하나는 걸린다?*/;start = hash_next(start)){
		if ((hash_find(spt, hash_bytes(va, sizeof(uint64_t))) == start)){
			break;
		}
	}


	if (hash_bytes(va, sizeof(uint64_t)) == spt->hash_list->hash){
		return page;
	}
	

	return page;
}

/* Insert PAGE into spt with validation. */
bool
spt_insert_page (struct supplemental_page_table *spt UNUSED,
		struct page *page UNUSED) {
	int succ = false;
	/* TODO: Fill this function. */
	// hash_insert를 통하여 spt에 페이지 삽입
	// hash_insert->hash_elem 반환 
	// *hash_insert (struct hash *, struct hash_elem *)
	return succ;
}

void
spt_remove_page (struct supplemental_page_table *spt, struct page *page) {
	vm_dealloc_page (page);+

	// hash_delete(struct hash *, struct hash_elem *)


	return true;
}

/* Get the struct frame, that will be evicted. */
static struct frame *
vm_get_victim (void) {
	struct frame *victim = NULL;
	 /* TODO: The policy for eviction is up to you. */

	return victim;
}

/* Evict one page and return the corresponding frame.
 * Return NULL on error.*/
static struct frame *
vm_evict_frame (void) {
	struct frame *victim UNUSED = vm_get_victim ();
	/* TODO: swap out the victim and return the evicted frame. */

	return NULL;
}

/* palloc() and get frame. If there is no available page, evict the page
 * and return it. This always return valid address. That is, if the user pool
 * memory is full, this function evicts the frame to get the available memory
 * space.*/
static struct frame *
vm_get_frame (void) {
	struct frame *frame = NULL;
	/* TODO: Fill this function. */

	ASSERT (frame != NULL);
	ASSERT (frame->page == NULL);
	return frame;
}

/* Growing the stack. */
static void
vm_stack_growth (void *addr UNUSED) {
}

/* Handle the fault on write_protected page */
static bool
vm_handle_wp (struct page *page UNUSED) {
}

/* Return true on success */
bool
vm_try_handle_fault (struct intr_frame *f UNUSED, void *addr UNUSED,
		bool user UNUSED, bool write UNUSED, bool not_present UNUSED) {
	struct supplemental_page_table *spt UNUSED = &thread_current ()->spt;
	struct page *page = NULL;
	/* TODO: Validate the fault */
	/* TODO: Your code goes here */
	return vm_do_claim_page (page);
	// vm_do_claim_page 함수
	// : 페이지의 VA를 프레임의 PA에 매핑하는 페이지 테이블 항목을 삽입하세요.
}

/* Free the page.
 * DO NOT MODIFY THIS FUNCTION. */
void
vm_dealloc_page (struct page *page) {
	destroy (page);
	free (page);
}

/* Claim the page that allocate on VA. */
bool
vm_claim_page (void *va UNUSED) {
	struct page *page = NULL;
	/* TODO: Fill this function */

	return vm_do_claim_page (page);
}

/* Claim the PAGE and set up the mmu. */
static bool
vm_do_claim_page (struct page *page) {
	struct frame *frame = vm_get_frame ();

	/* Set links */
	frame->page = page;
	page->frame = frame;

	/* TODO: Insert page table entry to map page's VA to frame's PA. */

	return swap_in (page, frame->kva);
}


/* Initialize new supplemental page table */
void
supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
	// 보조 페이지 테이블을 초기화
	// 보조 페이지 테이블에 사용할 데이터 구조를 선택할 수 있음
	// __do_fork 에서 호출됨 userprog/process.c
	if (!hash_init(&spt->hash_list, hash_bytes, cmp_page_less, NULL)){ //hash bytes 바꿔줘야 할 수도 있음
		return ;
	}
}

/* Copy supplemental page table from src to dst */
bool
supplemental_page_table_copy (struct supplemental_page_table *dst UNUSED,
		struct supplemental_page_table *src UNUSED) {

}

/* Free the resource hold by the supplemental page table */
void
supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. */


}
