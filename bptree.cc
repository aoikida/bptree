#include "bptree.h"

#define DATA_NUMBER 10000

void
delete_entry(NODE *node, int key, DATA *data, bool node_is_predecessor);


void
init_root(void)
{
	Root = NULL;
}


int
interactive()
{
  int key;

  std::cout << "Key: ";
  std::cin >> key;

  return key;
}


void
print_tree_core(NODE *n)
{
	printf("[");
	for (int i = 0; i < n->nkey; i++) {
		if (!n->isLeaf) print_tree_core(n->chi[i]);
		printf("%d", n->key[i]);
		if (i != n->nkey-1 && n->isLeaf) putchar(' ');
	}
	if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
	printf("]");
}


void
print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n"); fflush(stdout);
}


NODE *
alloc_leaf(NODE *parent)
{
	NODE *node = NULL;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}


TEMP *
alloc_temp(NODE *parent)
{
	TEMP *node = NULL;
	if (!(node = (TEMP *)calloc(1, sizeof(TEMP)))) ERR;
	node->isLeaf = true;
	node->nkey = N-1;

	return node;
}


NODE *
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}


void
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
	int i;

	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		}
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		for (int j = leaf->nkey; j > i; j--) {
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		}

		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;

		}

	leaf->nkey++;

	return;
}


void
insert_in_temp_leaf(TEMP *leaf, int key, DATA *data)
{
	int i;
	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		}
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		for (int j = leaf->nkey; j > i; j--) {
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		}

		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;

	}

	leaf->nkey++;

	return;
}


void
insert_in_internal(NODE *internal, int key, DATA *data)
{
	int i;

	if (key < internal->key[0]) {
		for (i = internal->nkey; i > 0; i--) {
			internal->key[i] = internal->key[i-1];
			internal->chi[i+1] = internal->chi[i];
		}
		internal->key[0] = key;
		internal->chi[1] = (NODE*)data;

	}
	else {
		for (i = 0; i < internal->nkey; i++) {
			if (key < internal->key[i]) break;
		}
		for (int j = internal->nkey; j > i; j--) {
			internal->key[j] = internal->key[j-1];
			internal->chi[j+1] = internal->chi[j];
		}

		internal->key[i] = key;
		internal->chi[i+1] = (NODE*)data;

	}

	internal->nkey++;

	return;
}


void
insert_in_temp_internal(TEMP *internal, int key, DATA *data)
{
	int i;

	if (key < internal->key[0]) {
		for (i = internal->nkey; i > 0; i--) {
			internal->key[i] = internal->key[i-1];
			internal->chi[i+1] = internal->chi[i];
		}
		internal->key[0] = key;
		internal->chi[1] = (NODE*)data;

	}
	else {
		for (i = 0; i < internal->nkey; i++) {
			if (key < internal->key[i]) break;
		}
		for (int j = internal->nkey; j > i; j--) {
			internal->key[j] = internal->key[j-1];
			internal->chi[j+1] = internal->chi[j];
		}

		internal->key[i] = key;
		internal->chi[i+1] = (NODE*)data;

	}

	internal->nkey++;

	return;
}


void
insert_in_parent(NODE *node, int key, DATA *split_node)
{
	NODE *parent_node = NULL;
	NODE *parent_split_node = NULL;
	NODE *internal_node = NULL;
	TEMP *temp = NULL;
	int i;
	int grand_parent_key;


	if (node == Root)
	{
		parent_node = alloc_leaf(NULL);
		parent_node->key[0] = key;
		parent_node->chi[0] = node;
		parent_node->chi[1] = (NODE *)split_node;
		parent_node->nkey = 1;
		Root = parent_node;
		parent_node->isLeaf = false;
		node->parent = parent_node;
		((NODE *)split_node)->parent = parent_node;

		return ;
	}

	parent_node = node->parent;
	((NODE*)split_node)->parent = parent_node;

	if (parent_node->nkey+1 < N)
	{

		parent_node->isLeaf = true;
		internal_node = find_leaf(Root, key);

		insert_in_internal(internal_node, key, split_node);

		parent_node->isLeaf = false;

	}
	else
	{
		temp = alloc_temp(NULL);

		for(i=0; i<N-1; i++){
			temp->key[i] = parent_node->key[i];
		}
		for(i=0; i<N; i++){
			temp->chi[i] = parent_node->chi[i];
		}

		insert_in_temp_internal(temp, key, split_node);

		for(i=0;i<N-1;i++){
			parent_node->key[i] = 0;
		}
		for(i=0;i<N;i++){
			parent_node->chi[i] = 0;
		}

		parent_split_node = alloc_leaf(NULL);
		parent_split_node->isLeaf = false;

		for(i=0; i<(N/2)+1; i++){
			parent_node->chi[i] = temp->chi[i];
		}
		for(i=0; i<(N/2); i++){
			parent_node->key[i] = temp->key[i];
		}

		parent_node->nkey = 2;
		grand_parent_key = temp->key[(N+1)/2];

		for(i=(N/2)+1; i<N+1; i++){
			parent_split_node->chi[i-(N/2)-1] = temp->chi[i];
			parent_split_node->chi[i-(N/2)-1]->parent = parent_split_node;
		}
		for(i=(N/2)+1; i<N; i++){
			parent_split_node->key[i-(N/2)-1] = temp->key[i];
		}

		free(temp);
		temp = NULL;

		parent_split_node->nkey = 1;

		insert_in_parent(parent_node, grand_parent_key, (DATA *)parent_split_node);

	}
}


void
insert(int key, DATA *data)
{
	NODE *leaf = NULL;
	NODE *split_leaf = NULL;
	TEMP *temp = NULL;
	int i;
	int parent_key;

	if (Root == NULL) {
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
  else {
    leaf = find_leaf(Root, key);
  }

	if (leaf->nkey < (N-1)) {
		insert_in_leaf(leaf, key, data);
	}
	else {
		temp = alloc_temp(NULL);

		for(i = 0; i<N-1; i++){
			temp->chi[i] = leaf->chi[i];
			temp->key[i] = leaf->key[i];
		}

		insert_in_temp_leaf(temp, key, data);

		split_leaf = alloc_leaf(NULL);

		split_leaf->chi[N-1] = leaf->chi[N-1];
		leaf->chi[N-1] = split_leaf;

		for(i = 0;i<N-1;i++){
			leaf->chi[i] = 0;
			leaf->key[i] = 0;
		}
		for(i = 0; i<(N/2); i++){
			leaf->chi[i] = temp->chi[i];
			leaf->key[i] = temp->key[i];
		}
		leaf->nkey = 2;

		for(i = (N/2); i<N; i++){
			split_leaf->chi[i-(N/2)] = temp->chi[i];
			split_leaf->key[i-(N/2)] = temp->key[i];
		}

		free(temp);
		temp = NULL;

		split_leaf->isLeaf = true;
		split_leaf->nkey = 2;

		parent_key = split_leaf->key[0];

		insert_in_parent(leaf, parent_key, (DATA *)split_leaf);

	}
}


NODE*
search_key(NODE * node, int key, int *count)
{
	int kid;

	if (node->isLeaf){
		for(int i = 0; i < node->nkey; i++){
			if (key == node->key[i]){
				*count  += 1;
				return 0;
			}
		}

		return 0;
	}

	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return search_key(node->chi[kid], key, count);

}


void
delete_entry_in_leaf(NODE *leaf, int key)
{

	for(int i = 0; i<leaf->nkey; i++){

    if(key == leaf->key[i]){
      if(i == leaf->nkey-1){
        leaf->key[i] = 0;
        leaf->chi[i] = 0;
      }
      else{
        for(int j = i; j < leaf->nkey - 1; j++){
  					leaf->key[j] = leaf->key[j+1];
  					leaf->chi[j] = leaf->chi[j+1];
  				}
  				leaf->key[leaf->nkey - 1] = 0;
  				leaf->chi[leaf->nkey - 1] = 0;
      }

			leaf->nkey--;

			return;

    }
  }
}


void
delete_entry_in_internal(NODE *internal, int key, bool node_is_predecessor)
{

	if(node_is_predecessor){

		for(int i = 0; i<internal->nkey; i++){

	    if(key == internal->key[i]){

	      if(i == (internal->nkey)-1){
	        internal->key[i] = 0;
	        internal->chi[i+1] = 0;
	      }
	      else{
	        for(int j = i; j < internal->nkey - 1; j++){
	  					internal->key[j] = internal->key[j+1];
	  					internal->chi[j+1] = internal->chi[j+2];
	  				}
	  				internal->key[internal->nkey - 1] = 0;
	  				internal->chi[internal->nkey] = 0;
	      }

	      internal->nkey --;

	      return;

	    }
	  }
	}

	else{
		for(int i = internal->nkey-1; i > -1; i--){

	    if(key == internal->key[i]){

	      if(i == (internal->nkey)-1){
	        internal->key[i] = 0;
	        internal->chi[i+1] = 0;
	      }
	      else{
	        for(int j = i; j < internal->nkey - 1; j++){
	  					internal->key[j] = internal->key[j+1];
	  					internal->chi[j+1] = internal->chi[j+2];
	  				}
	  				internal->key[internal->nkey - 1] = 0;
	  				internal->chi[internal->nkey] = 0;
	      }

	      internal->nkey --;

	      return;

	    }
	  }
	}
}


void
define_brother_node(NODE *node, NODE **brother_node, int *parent_key, bool * node_is_predecessor)
{
	int i;

	for (i = 0; i<(node->parent)->nkey+1; i++){

		if ((node->parent)->chi[i] == node){

			*brother_node = alloc_leaf(node->parent);

			if(i == 0){

				**brother_node = *(node->parent)->chi[1];
				free((node->parent)->chi[1]);
				(node->parent)->chi[1] = *brother_node;
				*parent_key = (node->parent)->key[i];
				*node_is_predecessor = true;

			}
			else{

				**brother_node = *(node->parent)->chi[i-1];
				free((node->parent)->chi[i-1]);
				(node->parent)->chi[i-1] = *brother_node;
				*parent_key = (node->parent)->key[i-1];
				*node_is_predecessor = false;

			}
		}
	}
}


void
coalesce_node(NODE *node, NODE *brother_node, int parent_key, bool node_is_predecessor)
{
	int i;
	int j;

	if(!node->isLeaf){
		if (node_is_predecessor){
			for(i = brother_node->nkey; i > 0; i--){
				brother_node->key[i] = brother_node->key[i-1];
			}
			for(i = brother_node->nkey+1; i > 0; i--){
				brother_node->chi[i] = brother_node->chi[i-1];
			}

			brother_node->chi[0] = node->chi[0];
			brother_node->key[0] = parent_key;
			brother_node->nkey++;
		}
		else
			insert_in_internal(brother_node, parent_key, (DATA*)node->chi[0]);

		for(j= 0; j<brother_node->nkey+1; j++){
			(brother_node->chi[j])->parent = brother_node;
		}

	}

	else{
		insert_in_leaf(brother_node,node->key[0],(DATA*)node->chi[0]);
		node->nkey--;
		brother_node->chi[N-1] = node->chi[N-1];
	}

	if(node_is_predecessor)
		(node->parent)->chi[0] = brother_node;

	free(node);
	node = NULL;


	delete_entry(brother_node->parent, parent_key, (DATA*)brother_node, node_is_predecessor);


}


void
redistribution_entry(NODE *node, NODE *brother_node, int parent_key, bool node_is_predecessor)
{
	int i;
	int j;

	if(!node_is_predecessor){
		if(!node->isLeaf){

			node->key[0] = parent_key;
			node->chi[1] = node->chi[0];
			node->chi[0] = brother_node->chi[N-1];
			(node->chi[0])->parent = node;
			node->nkey ++;

			for(i = 0; i < (brother_node->parent)->nkey; i++){
				if (brother_node == (brother_node->parent)->chi[i])
					(brother_node->parent)->key[i] = brother_node->key[N-2];
			}

			brother_node->key[N-2] = 0;
			brother_node->chi[N-1] = 0;
			brother_node->nkey --;

			for(j = 0; j < brother_node->nkey+1; j++){
				(brother_node->chi[j])->parent = brother_node;
			}

		}
		else{

			node->key[1] = node->key[0];
			node->key[0] = brother_node->key[N-2];

			node->chi[1] = node->chi[0];
			node->chi[0] = brother_node->chi[N-2];
			node->nkey ++;

			for(i = 0; i < (brother_node->parent)->nkey; i++){
				if (brother_node == (brother_node->parent)->chi[i])
					(brother_node->parent)->key[i] = brother_node->key[N-2];
			}

			brother_node->key[N-2] = 0;
			brother_node->chi[N-2] = 0;
			brother_node->nkey --;

		}
	}
	else {
		if(!node->isLeaf){

			node->key[0] = parent_key;
			node->chi[1] = brother_node->chi[0];
			(node->chi[1])->parent = node;
			node->nkey ++;


			for(i = 1; i < (brother_node->parent)->nkey+1; i++){
				if (brother_node == (brother_node->parent)->chi[i])
					(brother_node->parent)->key[i-1] = brother_node->key[0];
			}

			for(i = 0; i<brother_node->nkey; i++){
				brother_node->chi[i] = brother_node->chi[i+1];
			}

			for(i = 0; i<brother_node->nkey-1; i++){
				brother_node->key[i] = brother_node->key[i+1];
			}
			brother_node->nkey --;

			for(j = 0; j<brother_node->nkey+1; j++){
				(brother_node->chi[j])->parent = brother_node;
			}

		}

		else{

			node->key[1] = brother_node->key[0];
			node->chi[1] = brother_node->chi[0];
			node->nkey ++;

			for(i = 1; i < (brother_node->parent)->nkey+1; i++){
				if (brother_node == (brother_node->parent)->chi[i])
					(brother_node->parent)->key[i-1] = brother_node->key[1];
			}

			for(i = 0; i<brother_node->nkey-1; i++){
				brother_node->key[i] = brother_node->key[i+1];
				brother_node->chi[i] = brother_node->chi[i+1];
			}

			brother_node->nkey --;

		}
	}
}


void
delete_entry_in_node(NODE *node, int key, bool node_is_predecessor)
{
	if(node->isLeaf)	
		delete_entry_in_leaf(node, key);
	
	else	
		delete_entry_in_internal(node, key, node_is_predecessor);
}


void
delete_entry(NODE *node, int key, DATA *data, bool node_is_predecessor)
{
	NODE *brother_node = NULL;
	int parent_key;


	delete_entry_in_node(node, key, node_is_predecessor);

	if (node->nkey == 1 && node == Root && node->isLeaf)
		return;

	if (node->nkey == 0 && node == Root && node->isLeaf){
		free(node);
		node = NULL;
		return;
	}

	if(node->nkey == 0 && node == Root){

		Root = Root->chi[0];
		free(node);
		node = NULL;
		return ;

	}
	else if ((node->isLeaf == true && node->nkey < (int)N/2) ||
           ( node->isLeaf == false && node->nkey < (int)(N+1)/2-1)){

		define_brother_node(node, &brother_node, &parent_key, &node_is_predecessor);

		if (brother_node->nkey < N-1)
			coalesce_node(node, brother_node, parent_key, node_is_predecessor);

		else
			redistribution_entry(node, brother_node, parent_key, node_is_predecessor);

	}
}


void
delete_(int key, DATA *data)
{
	bool node_is_predecessor = false;
	NODE *leaf_node_have_delete_key = NULL;

	leaf_node_have_delete_key = find_leaf(Root, key);

	delete_entry(leaf_node_have_delete_key, key, data, node_is_predecessor);

}


int
main(int argc, char *argv[])
{
	int key;
	int count = 0;
	int insert_number;
	int delete_number;
	int delete_number_i;

  	std::vector<int> vec;
	srand(10);

	init_root();


	printf("-----Insert-----\n");

	for (key = 1; key < DATA_NUMBER+1; key++) {
    　　　　　　  vec.push_back(rand()+1);
  	}

	for(key = 0; key < DATA_NUMBER; key++){
		insert(vec[key], NULL);
	}

	print_tree(Root);


	printf("-----Search-----\n");

	for(key = 0; key < DATA_NUMBER; key++){
		search_key(Root, vec[key], &count);
	}
	printf("%d\n", count);


	while (true) {

		delete_number_i = rand() % vec.size();
		delete_number = vec[delete_number_i];
		//printf("delete_key : %d\n", delete_number);
		vec.erase(vec.begin()+delete_number_i);
		delete_(delete_number, NULL);
		//print_tree(Root);

		insert_number = rand() + 1;
		vec.push_back(insert_number);
		//printf("insert_key : %d\n", insert_number);
		insert(insert_number, NULL);
		//print_tree(Root);

  	}

  	return 0;

}
