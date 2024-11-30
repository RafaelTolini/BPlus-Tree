#include <stdio.h>
#include <stdlib.h>

#define FOLHA 1
#define MAX_CHAVES 3
#define MAX_PONTEIROS 4

typedef struct No {
    int chaves[MAX_CHAVES];
    struct No* ponteiros[MAX_PONTEIROS];
    int ehFolha;
    int numChaves;
    struct No* pai;
} No;

int fezSplit = 0;


No* criaNo(int folha, No* pai);
No* cisaoFolha(No* raiz, int valorMeio);
No* insereChave(No* raiz, int chave, int ehFolha);
int encontraIndexInsercaoPonteiro(No* raiz, int chave);
int encontraIndexInsercaoFolha(No* raiz, int chave);
int verificaEhFolha(No* raiz);
void swapNo(No* raiz);

No* criaNo(int folha, No* pai) {
    
    No* novo = (No*)malloc(sizeof(No));
    if (!novo) return NULL;
    for (int i = 0; i < MAX_CHAVES; i++) novo->chaves[i] = -1;
    for (int i = 0; i < MAX_PONTEIROS; i++) novo->ponteiros[i] = NULL;
    novo->ehFolha = folha;
    novo->numChaves = 0;
    novo->pai = pai;
    
    return novo;
}

// Encontra posição para inserir ordenadamente
int encontraIndexInsercaoFolha(No* raiz, int chave){
    int i = raiz->numChaves - 1;
    while (i >= 0 && raiz->chaves[i] > chave) {
        raiz->chaves[i + 1] = raiz->chaves[i]; // Desloca a chave maior
        i--;
    }
    return i + 1;
}

//A função de encontrar o filho para inserir não estava funcionando;
//A condição raiz->chaves[1] <= chave deveria retornar 2 mas retornava 1.
//Mudei pra ficar parecido com a busca de índice de chave.
int encontraIndexInsercaoPonteiro(No* raiz, int chave){
    int i = raiz->numChaves;
    while (i > 0 && raiz->chaves[i-1] > chave)i--;
    return i;
}

//Aqui se você encontra um ponteiro não vazio, deveria retornar !FOLHA
//caso contrário, retorna FOLHA. Estava invertido.
int verificaEhFolha(No* raiz){
    for(int i = 0; i < MAX_PONTEIROS; i++){
        if(raiz->ponteiros[i]) return !FOLHA;
    }
    return FOLHA;
}

void swapNo(No* raiz){
    raiz->chaves[0] = raiz->chaves[1]; 
    raiz->chaves[1] = raiz->chaves[2]; 
    raiz->chaves[2] = -1;
    raiz->numChaves--;
}


No* insereChave(No* raiz, int chave, int ehFolha) {

    if (!raiz) {
        raiz = criaNo(ehFolha, NULL);

        if (!raiz) return NULL;
    }
    
    if(raiz->ehFolha){
        int idxFolha = encontraIndexInsercaoFolha(raiz, chave);
        raiz->chaves[idxFolha] = chave;
        raiz->numChaves++;
        if(raiz->numChaves== 3){
            return cisaoFolha(raiz, raiz->chaves[1]);
        } 
    }
    //Caso um novo nó não-folha seja criado é preciso verificar se ele está vazio
    //antes de tentar acessar raiz->ponteiros[idxPonteiro] (seg fault).
    else if(raiz->numChaves>0){
        int idxPonteiro = encontraIndexInsercaoPonteiro(raiz, chave);
        int ponteiroEhFolha = verificaEhFolha(raiz->ponteiros[idxPonteiro]);
        //Aqui tirei onde o nó ajustava o ponteiro que foi modificado, porque são muitos casos
        //de retorno quando tem cisão e nem sempre o nó retornado era o que deveria estar naquela
        //posição. Agora se preciso os ponteiros do pai são ajustados pelo filho na função cisão.
        No* retorno = insereChave(raiz->ponteiros[idxPonteiro], chave, ponteiroEhFolha);


        //A flag fezSplit é ligada quando a raiz sofre split (e não é folha). Ela é usada para,
        //caso tenha alguma modificação na folha que propague até a raiz e ocorra a cisão,
        //ser possível saber no caminho de volta das chamadas de função qual era a antiga raiz,
        //para poder retornar seu pai (a nova raiz).
        if(fezSplit){
            fezSplit=0;
            No* temp = raiz;
            while(temp->pai!=NULL)temp=temp->pai;
            return temp;
        }
        else if(retorno == raiz->pai)return raiz->pai;
       
    }
    return raiz;
}

No* insereChaveInterno(No* raiz, int chave){
    int idxFolha = encontraIndexInsercaoFolha(raiz, chave);
    raiz->chaves[idxFolha] = chave;
    raiz->numChaves++;
    if(raiz->numChaves == 3){
        return cisaoFolha(raiz, raiz->chaves[1]);
    }
    return raiz;
}


No* cisaoFolha(No* raiz, int valorMeio){
    if(!(raiz->pai)){
        No* novaRaiz = criaNo(!FOLHA, NULL);
        novaRaiz->chaves[0] = valorMeio;
        
        No** filhoMenor = &(novaRaiz->ponteiros[0]);
        //Aqui o novo irmão criado da antiga raiz deve ser folha só caso a raiz
        //seja folha também. Se a árvore tem 3 níveis a raíz vai se dividir em
        //dois nós internos.
        *filhoMenor = insereChave(NULL, raiz->chaves[0], raiz->ehFolha);
        (*filhoMenor)->pai = novaRaiz;
        novaRaiz->ponteiros[1] = raiz;

        //Se a raiz não for folha, é preciso dividir os ponteiros dela também.
        //Aqui ele copia a chave 0 para o filhoMenor, se livra dessa chave com o swap,
        //Dá os seus dois primeiros ponteiros para o filhoMenor, ajustando também o pai deles,
        //e passa seus dois ultimos ponteiros para a posição de seus dois primeiros. Por ultimo
        //liga a flag fezSplit. Para fazer esses ajustes o número máximo de ponteiros foi
        //mudado de 3 para 4. Dando espaço para o overflow assim como a terceira chave faz.
        if(!raiz->ehFolha){
            (*filhoMenor)->chaves[0] = raiz->chaves[0];
            swapNo(raiz);
           
            (*filhoMenor)->ponteiros[0]=raiz->ponteiros[0];
            (*filhoMenor)->ponteiros[1]=raiz->ponteiros[1];
            raiz->ponteiros[0]->pai=(*filhoMenor);
            raiz->ponteiros[1]->pai=(*filhoMenor);
            (*filhoMenor)->numChaves++;
          
            raiz->ponteiros[0]=raiz->ponteiros[2];
            raiz->ponteiros[1]=raiz->ponteiros[3];
            raiz->ponteiros[2]=raiz->ponteiros[3]=NULL;

            fezSplit = 1;
        }
        swapNo(raiz);
        novaRaiz->numChaves++;
        raiz->pai = novaRaiz;
        return novaRaiz;
    }
    else{
        //No início encontramos o índice do ponteiro onde o filhoMenor vai entrar no pai.
        //Caso a chave entre no ponteiro 1, idx será 2 e caso entre no ponteiro 2
        //será 3 (e causará cisão acima dele).
        int idx = encontraIndexInsercaoPonteiro(raiz->pai, raiz->chaves[0])+1;

        No* filhoMenor = criaNo(FOLHA, raiz->pai);
        filhoMenor = insereChave(filhoMenor, raiz->chaves[0], FOLHA);
        filhoMenor->ehFolha = raiz->ehFolha;
        raiz->pai->ponteiros[idx] = filhoMenor;
        swapNo(raiz);
        int temp = raiz->pai->ponteiros[idx]->chaves[0];
        raiz->pai->ponteiros[idx]->chaves[0] = raiz->pai->ponteiros[idx-1]->chaves[0];
        raiz->pai->ponteiros[idx]->chaves[1] = raiz->pai->ponteiros[idx-1]->chaves[1];
        raiz->pai->ponteiros[idx-1]->chaves[0] = temp;
        raiz->pai->ponteiros[idx]->numChaves++;
        raiz->pai->ponteiros[idx-1]->numChaves--;

        //Caso tenha feito a cisão de um nó interno, precisamos ajustar os ponteiros dele.
        //Tiramos o nó que vamos mandar para o pai, ajustamos os ponteiros entre raiz e
        //filhoMenor de forma parecida com o que foi feito na cisão de uma raíz não-folha.
        if(!raiz->ehFolha){
            swapNo(filhoMenor);
                   
            filhoMenor->ponteiros[0]=raiz->ponteiros[2];     
            filhoMenor->ponteiros[1]=raiz->ponteiros[3];
            raiz->ponteiros[2]->pai=filhoMenor;
            raiz->ponteiros[3]->pai=filhoMenor;
          
            raiz->ponteiros[2]=raiz->ponteiros[3]=NULL;
        }
        
        raiz->pai->ponteiros[idx] = filhoMenor;
        //Como o ajuste de nós em níveis mais altos depende que os nós abaixo dele estejam
        //ajustados, faz mais sentido só enviar a chave do meio para o pai e lidar com
        //possíveis cisões propagadas quando terminarmos a cisão do nó atual.
        //Por isso optei por botar essa função no final.
        insereChaveInterno(raiz->pai, valorMeio);

        return raiz;
    }
}




void imprimeArvore(No* no) {
    if (no == NULL) return;
    printf("No: %p, chaves: [", no);
    for (int i = 0; i < no->numChaves; i++) {
        printf("%d", no->chaves[i]);
        if (i < no->numChaves - 1) printf(", ");
    }
    printf("], ptrs: [");
    for (int i = 0; i <= no->numChaves; i++) {
        if (no->ponteiros[i] != NULL)
            printf("%p", no->ponteiros[i]);
        else
            printf("NULL");
        if (i < no->numChaves) printf(", ");
    }
    printf("]\n");

    for (int i = 0; i <= no->numChaves; i++) {
        if (no->ponteiros[i] != NULL)
            imprimeArvore(no->ponteiros[i]);
    }
}

int main() {
    No* raiz = NULL;
    
    raiz = insereChave(raiz, 10,FOLHA);
    raiz = insereChave(raiz, 15, !FOLHA);
    raiz = insereChave(raiz, 13, !FOLHA);
    raiz = insereChave(raiz, 5, !FOLHA);
    raiz = insereChave(raiz, 20, !FOLHA);
    raiz = insereChave(raiz, 25, !FOLHA);
    // raiz = insereChave(raiz, 26, !FOLHA);
    // raiz = insereChave(raiz, 27, !FOLHA);
    // raiz = insereChave(raiz, 28, !FOLHA);
    // raiz = insereChave(raiz, 29, !FOLHA);

    imprimeArvore(raiz);

    return 0;
}