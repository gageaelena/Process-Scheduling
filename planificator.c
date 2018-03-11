#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct proces
{
    char *name;
    int time, prio;
}proces;

typedef struct node //pt liste simplu inlant
{
    proces proc;
    struct node *next;
} node;

typedef struct dublu
{
    proces proc;
    struct dublu *next;
    struct dublu *prev;
} dublu;

void init(node **head)
{
    *head = NULL;
}

void initdublu(dublu **head)
{
    *head = NULL;
}

void show(node *head, FILE *fo) // afiseaza procesul din running
{
    //head = running;

    if (head == NULL)
    {
        fprintf(fo, "\n");
        return;
    }
    //Nu exista procese in RUNNING! Afisam doar linie goala.
    else
        fprintf(fo, "%s %d\n", head->proc.name, head->proc.time);
}

void add(node **head, proces proc) // adauga in coada ready
{
    //s=name, t=time, pr=prio;
    node *p;
    p = (node *)malloc(sizeof(node));

    p->proc.name = (char *)malloc(sizeof(char) * 20);
    p->proc.time = proc.time;
    p->proc.prio = proc.prio;
    strcpy(p->proc.name, proc.name);
    p->next = *head;
    *head = p;
}

void wait(node **prun, dublu **head2) //trece proc din running in waiting
{
    // head1=running, head2=waiting

    dublu *aux;
    dublu *new = (dublu *)malloc(sizeof(dublu));

    new->prev = NULL;
    new->next = NULL;
    new->proc.name = (char *)malloc(sizeof(char) * 20);
    strcpy(new->proc.name, (*prun)->proc.name);
    new->proc.time = (*prun)->proc.time;
    new->proc.prio = (*prun)->proc.prio;

    if (*head2 == NULL)
    {
        *head2 = new;
        free((*prun)->proc.name);
        free(*prun);
        *prun = NULL;
        return;
    }

    aux = *head2;
    while (aux->next)
    {
        aux = aux->next;
    }
    aux->next = new;
    new->prev = aux;

    //elimin procesul din running adaugat in waiting;
    free((*prun)->proc.name);
    free(*prun);
    *prun = NULL;
}

node *delReady(node **head)
{
    node *curent, *aux, *urmator;
    curent = *head;

    if (curent == NULL)
    {
        return NULL;
    }
    urmator = (*head)->next;
    //printf("\n\n%s\n\n", urmator->name);
    if (urmator == NULL)
    {
        aux = curent;
        (*head) = NULL;
        return aux;
    }

    while (urmator->next != NULL)
    {
        curent = urmator;
        urmator = urmator->next;
    }

    aux = urmator;
    curent->next = NULL;

    return aux;
}

node *SearchForMin(node **ready)
{
    node *aux;

    int timemin;

    if (ready == NULL)
        return 0;
    else
    {

        aux = *ready;

        timemin = (*ready)->proc.time;
        node *nodeTimeMin = *ready;
        //printf("\ntimemin: %d\n", timemin);

        while (aux != NULL)
        {
            if (aux->proc.time > timemin)
            {
                aux = aux->next;
            }
            else
            {
                timemin = aux->proc.time;
                nodeTimeMin = aux;
                //printf("\ntimemin: %d\n", timemin);
                aux = aux->next;
            }
        }
        return nodeTimeMin;
    }
}


void DelElemCoada(node **ready, char *nume)
{
    node *curent, *prev;

    if (strcmp(nume, (*ready)->proc.name) == 0)
    {
        prev = (*ready);
        (*ready) = (*ready)->next;
    }
    else
    {
        prev = *ready;
        curent = (*ready)->next;

        while (curent->next != NULL && strcmp(curent->proc.name, nume) != 0)
        {
            prev = curent;
            curent = curent->next;
        }

        if (curent->next == NULL)
        {
            prev->next = NULL;
        }
        else
        {
            prev->next = curent->next;
        }
    }
}

void tick(node **run, node **ready)
{

    if (*run == NULL)
    {
        if ((*ready) != NULL)
            *run = delReady(ready);
        //printf("in tick \n\n%s %d %d\n\n", (*run)->proc.name, (*run)->proc.time, (*run)->proc.prio);
    }
    else
    {
        (*run)->proc.time--;

        if ((*run)->proc.time == 0)
        {
            free((*run)->proc.name);
            free((*run));
            *run = NULL;
            //printf("in tick \n\n%s %d %d\n\n", (*run)->proc.name, (*run)->proc.time, (*run)->proc.prio);
            *run = delReady(ready);
        }
    }
}

void tickSJF(node **run, node **ready)
{
    if (*run == NULL)
    {
        if ((*ready) != NULL)
        {
            *run = SearchForMin(ready);
            DelElemCoada(ready, (*run)->proc.name);
        }
        //printf("in tick \n\n%s %d %d\n\n", (*run)->proc.name, (*run)->proc.time, (*run)->proc.prio);
    }
    else
    {
        (*run)->proc.time--;

        if ((*run)->proc.time == 0)
        {
            free((*run)->proc.name);
            free(*run);
            *run = NULL;
            //printf("in tick \n\n%s %d %d\n\n", (*run)->proc.name, (*run)->proc.time, (*run)->proc.prio);
            if ((*ready) != NULL)
            {
                *run = SearchForMin(ready);
                DelElemCoada(ready, (*run)->proc.name);
            }
        }
    }
}

void tickRR(node **run, node **ready, int cuanta, int *count)
{
    node *aux = *run;

    if (*run == NULL)
    { //printf("coada nula run");
        if ((*ready) != NULL)
        {
            *run = delReady(ready);
            *count = 0;
        }
    }
    else
    {

        if ((*run)->proc.time != 0)
        {
            (*run)->proc.time--;
            *count = *count + 1;

            if ((*run)->proc.time == 0)
            {
                *count = 0;
                free((*run)->proc.name);
                free(*run);
                *run = NULL;
                *run = delReady(ready);
            }
            else
            {

                if ((*count) == cuanta)
                {
                    add(ready, aux->proc);
                    *count = 0;
                    free((*run)->proc.name);
                    free(*run);
                    *run = NULL;
                    if ((*ready) != NULL)
                    {
                        *run = delReady(ready);
                    }
                }
            }
        }
    }
}

node *SearchForMaxPrio(node **ready)
{
    node *aux;
    //int timeMin;
    node *nodePrioMax = NULL;
    int priomax;

    if ((*ready) == NULL)
        return NULL;
    else
    {

        aux = (*ready);
        priomax = (*ready)->proc.prio;
        nodePrioMax = aux;

        while (aux != NULL)
        {
            if (aux->proc.prio >= priomax)
            {
                nodePrioMax = aux;
                priomax = aux->proc.prio;
            }

            aux = aux->next;
        }
    }

    return nodePrioMax;
}

void tickPP(node **run, node **ready, int cuanta, int *count)
{
    if (*run == NULL)
    {
        if (*ready != NULL)
        {

            *run = SearchForMaxPrio(ready);
            DelElemCoada(ready, (*run)->proc.name);
            *count = 0;
        }

        return;
    }

    node *auxp = SearchForMaxPrio(ready);
    int prioCmp = -1;
    if (auxp != NULL)
        prioCmp = auxp->proc.prio;

    if ((*run)->proc.prio < prioCmp)
    {
        (*run)->proc.time--;
        if ((*run)->proc.time != 0)
            add(ready, (*run)->proc);

        free((*run)->proc.name);
        free(*run);
        (*run) = auxp;
        if (*run != NULL)
            DelElemCoada(ready, (*run)->proc.name);
        *count = 0;
        return;
    }

    if (*count < cuanta)
    {
        (*run)->proc.time--;
        (*count)++;
        ;
    }

    if ((*run)->proc.time == 0)
    {
        *count = 0;

        free((*run)->proc.name);
        free(*run);
        (*run) = auxp;
        if (*run != NULL)
            DelElemCoada(ready, (*run)->proc.name);
    }
    else if (*count == cuanta)
    {
        *count = 0;

        if ((*run)->proc.prio == prioCmp)
        {
            add(ready, (*run)->proc);
            free((*run)->proc.name);
            free(*run);
            (*run) = auxp;
            if (*run != NULL)
                DelElemCoada(ready, (*run)->proc.name);
        }
    }
}

void event(dublu **head1, node **head2, char *name) //trezeste procsul din waiting si il baga in ready
{

    //head1 = waiting, head2 = ready;

    dublu *aux;
    aux = *head1;

    while (aux && strcmp(aux->proc.name, name) != 0)
    {
        aux = aux->next;
    }

    if (aux == NULL)
        ;
    //printf("nu exista procesul in asteptare");
    else
    {
        add(head2, aux->proc);
    }

    // pt cazul in care trebuie sa scot din lista waiting chiar head-ul (aux este primul nod din lista)
    if (*head1 == aux)
    {
        *head1 = (*head1)->next;

        if (*head1 != NULL)
            (*head1)->prev = NULL;
    }
    else if (aux->next == NULL)
    {
        // pt cazul in care aux este ultimul nod din lista
        aux->prev->next = NULL;
    }
    else
    {
        // pt cazul in care aux este in interiorul listei
        aux->next->prev = aux->prev;
        aux->prev->next = aux->next;
    }

    if (aux != NULL)
    {
        free(aux->proc.name);
        free(aux);
        aux = NULL;
    }
}

void printList(node *head)
{
    node *nodCrt;
    for (nodCrt = head; nodCrt != NULL; nodCrt = nodCrt->next)
    {
        printf("%s %d %d; ", nodCrt->proc.name, nodCrt->proc.time, nodCrt->proc.prio);
    }
    printf("\n");
}

void printListDublu(dublu *head)
{
    dublu *nodCrt;
    for (nodCrt = head; nodCrt != NULL; nodCrt = nodCrt->next)
    {
        printf("%s %d %d; ", nodCrt->proc.name, nodCrt->proc.time, nodCrt->proc.prio);
    }
    printf("\n");
}

void listsimplu(node *head) // pt liste simplu inlant
{
    //printf("procesul este:");

    while (head != NULL)
    {

        printf("%s  ", head->proc.name);
        printf("%d  ", head->proc.time);
        printf("%d  ", head->proc.prio);
        printf("\n");
        head = head->next;
    }
}

void listdublu(dublu *head) //pentru liste dublu inlant
{
    printf("waitin' :\n");

    while (head != NULL)
    {

        printf("%s  ", head->proc.name);
        printf("%d  ", head->proc.time);
        printf("%d  ", head->proc.prio);
        printf("\n");
        head = head->next;
    }
}

void readFirstLine(FILE *f, int *tipAlgo, int *cuanta, int *nrNivPrior)
{
    *tipAlgo = *cuanta = *nrNivPrior = -1;

    char buffer[101];

    fgets(buffer, 101, f);

    // tip algoritm
    char *p = strtok(buffer, "  \n");
    *tipAlgo = atoi(p);

    // cuanta (daca exista)
    p = strtok(NULL, " \n");
    if (p != NULL)
    {
        *cuanta = atoi(p);

        p = strtok(NULL, "  \n");
        if (p != NULL)
        {
            *nrNivPrior = atoi(p);
        }
    }
}

void FCFS(FILE *f, FILE *fo)
{
    char buffer[101];
    char *p;

    proces proc;
    proc.name = (char*)malloc(sizeof(char)*20);

    node *ready, *running;
    dublu *waiting;

    running = NULL;
    init(&ready);
    initdublu(&waiting);

    while (fgets(buffer, 101, f) != NULL)
    {
        p = strtok(buffer, "  \n");

        if (p == NULL)
            break;

        switch (p[0])
        {
        case 't':

            //printf("tick\n");
            tick(&running, &ready);
            break;

        case 'a': // add
            // nume
            p = strtok(NULL, "  \n");
            strcpy(proc.name, p);

            // timp de rulare
            p = strtok(NULL, "  \n");
            proc.time = atoi(p);

            // prioritate
            p = strtok(NULL, "  \n");
            proc.prio = atoi(p);

            // printf("add: %s %d %d\n", nume, x, y);
            add(&ready, proc);
            tick(&running, &ready);
            break;

        case 'm': // multiple add
            // nume
            p = strtok(NULL, "  \n");

            //printf("ma: ");

            while (p != NULL)
            {
                strcpy(proc.name, p);

                // timp de rulare
                p = strtok(NULL, "  \n");
                proc.time = atoi(p);

                // prioritate
                p = strtok(NULL, "  \n");
                proc.prio = atoi(p);
                add(&ready, proc);

                // numele urmatorului proces (daca exista)
                p = strtok(NULL, "  \n");
                //printf("%s %d %d; ", nume, x, y);
            }
            tick(&running, &ready);
            printf("\n");
            break;

        case 'w': //wait

            // printf("wait\n");
            wait(&running, &waiting);
            tick(&running, &ready);
            break;

        case 'e': //event
            // nume
            p = strtok(NULL, " \n");
            strcpy(proc.name, p);

            //printf("event: %s\n", nume);
            event(&waiting, &ready, proc.name);
            tick(&running, &ready);
            break;

        case 's': //show
            //printf("show\n");
            show(running, fo);
            break;

        default:
            break;
        }
    }

    free(proc.name);

    node *aux = ready;
    while (aux != NULL)
    {
        ready = ready->next;
        free(aux->proc.name);
        free(aux);
        aux = ready;
    }

    dublu *auxD = waiting;
    while (auxD != NULL)
    {
        waiting = waiting->next;
        free(auxD->proc.name);
        free(auxD);
        auxD = waiting;
    }

    if (running != NULL)
    {
        free(running->proc.name);
        free(running);
    }
}

void SJF(FILE *f, FILE *fo)
{
    char buffer[101];
    char *p;

    proces proc;
    proc.name = (char*)malloc(sizeof(char)*20);

    node *ready, *running;
    dublu *waiting;

    running = NULL;
    init(&ready);
    initdublu(&waiting);

    while (fgets(buffer, 101, f) != NULL)
    {
        p = strtok(buffer, "  \n");

        if (p == NULL)
            break;

        switch (p[0])
        {
        case 't':

            //printf("tick\n");
            tickSJF(&running, &ready);
            break;

        case 'a': // add
            // nume
            p = strtok(NULL, "  \n");
            strcpy(proc.name, p);

            // timp de rulare
            p = strtok(NULL, "  \n");
            proc.time = atoi(p);

            // prioritate
            p = strtok(NULL, "  \n");
            proc.prio = atoi(p);

            // printf("add: %s %d %d\n", nume, x, y);
            add(&ready, proc);

            tickSJF(&running, &ready);
            break;

        case 'm': // multiple add
            // nume
            p = strtok(NULL, "  \n");

            //printf("ma: ");

            while (p != NULL)
            {
                strcpy(proc.name, p);

                // timp de rulare
                p = strtok(NULL, "  \n");
                proc.time = atoi(p);

                // prioritate
                p = strtok(NULL, "  \n");
                proc.prio = atoi(p);
                add(&ready, proc);
                // numele urmatorului proces (daca exista)
                p = strtok(NULL, "  \n");
                //printf("%s %d %d; ", nume, x, y);
            }
            tickSJF(&running, &ready);
            printf("\n");
            break;

        case 'w': //wait

            // printf("wait\n");
            wait(&running, &waiting);
            tickSJF(&running, &ready);
            break;

        case 'e': //event
            // nume
            p = strtok(NULL, " \n");
            strcpy(proc.name, p);

            //printf("event: %s\n", nume);
            event(&waiting, &ready, proc.name);
            tickSJF(&running, &ready);
            break;

        case 's': //show
            //printf("show\n");
            show(running, fo);
            break;

        default:
            break;
        }
    }

    free(proc.name);

    node *aux = ready;
    while (aux != NULL)
    {
        ready = ready->next;
        free(aux->proc.name);
        free(aux);
        aux = ready;
    }

    dublu *auxD = waiting;
    while (auxD != NULL)
    {
        waiting = waiting->next;
        free(auxD->proc.name);
        free(auxD);
        auxD = waiting;
    }

    if (running)
    {
        free(running->proc.name);
        free(running);
    }
}

void RR(FILE *f, FILE *fo, int cuanta)
{
    int *count = (int *)malloc(sizeof(int));
    *count = 0;

    char buffer[101];
    char *p;
    
    proces proc;
    proc.name = (char*)malloc(sizeof(char)*20);

    node *ready, *running;
    dublu *waiting;

    running = NULL;
    init(&ready);
    initdublu(&waiting);

    while (fgets(buffer, 101, f) != NULL)
    {
        p = strtok(buffer, "  \n");

        if (p == NULL)
            break;

        switch (p[0])
        {
        case 't':

            //printf("tick\n");
            tickRR(&running, &ready, cuanta, count);
            break;

        case 'a': // add
            // nume
            p = strtok(NULL, "  \n");
            strcpy(proc.name, p);

            // timp de rulare
            p = strtok(NULL, "  \n");
            proc.time = atoi(p);

            // prioritate
            p = strtok(NULL, "  \n");
            proc.prio = atoi(p);

            // printf("add: %s %d %d\n", nume, x, y);
            add(&ready, proc);
            tickRR(&running, &ready, cuanta, count);
            break;

        case 'm': // multiple add
            // nume
            p = strtok(NULL, "  \n");

            //printf("ma: ");

            while (p != NULL)
            {
                strcpy(proc.name, p);

                // timp de rulare
                p = strtok(NULL, "  \n");
                proc.time = atoi(p);

                // prioritate
                p = strtok(NULL, "  \n");
                proc.prio = atoi(p);
                add(&ready, proc);


                // numele urmatorului proces (daca exista)
                p = strtok(NULL, "  \n");
                //printf("%s %d %d; ", nume, x, y);
            }
            tickRR(&running, &ready, cuanta, count);
            printf("\n");
            break;

        case 'w': //wait

            // printf("wait\n");
            wait(&running, &waiting);
            tickRR(&running, &ready, cuanta, count);
            break;

        case 'e': //event
           // nume
            p = strtok(NULL, " \n");
            strcpy(proc.name, p);

            //printf("event: %s\n", nume);
            event(&waiting, &ready, proc.name);
            tickRR(&running, &ready, cuanta, count);
            break;

        case 's': //show
            //printf("show\n");
            show(running, fo);
            break;

        default:
            break;
        }
    }

    free(proc.name);
    free(count);

    node *aux = ready;
    while (aux != NULL)
    {
        ready = ready->next;
        free(aux->proc.name);
        free(aux);
        aux = ready;
    }

    dublu *auxD = waiting;
    while (auxD != NULL)
    {
        waiting = waiting->next;
        free(auxD->proc.name);
        free(auxD);
        auxD = waiting;
    }

    if (running)
    {
        free(running->proc.name);
        free(running);
    }
}

void PP(FILE *f, FILE *fo, int cuanta, int nrNivPrior)
{
    int *count = (int *)malloc(sizeof(int));
    *count = 0;

    char buffer[101];
    char *p;
    proces proc;
    proc.name = (char*)malloc(sizeof(char)*20);

    node *ready, *running;
    dublu *waiting;

    running = NULL;
    init(&ready);
    initdublu(&waiting);

    while (fgets(buffer, 101, f) != NULL)
    {
        p = strtok(buffer, "  \n");

        if (p == NULL)
            break;

        printf("cmd:%c\n", p[0]);

        switch (p[0])
        {
        case 't':

            //printf("tick\n");
            tickPP(&running, &ready, cuanta, count);
            break;

        case 'a': // add
            // nume
            p = strtok(NULL, "  \n");
            strcpy(proc.name, p);

            // timp de rulare
            p = strtok(NULL, "  \n");
            proc.time = atoi(p);

            // prioritate
            p = strtok(NULL, "  \n");
            proc.prio = atoi(p);

            // printf("add: %s %d %d\n", nume, x, y);
            add(&ready, proc);
            tickPP(&running, &ready, cuanta, count);
            break;

        case 'm': // multiple add
            // nume
            p = strtok(NULL, "  \n");

            //printf("ma: ");

            while (p != NULL)
            {
                strcpy(proc.name, p);

                // timp de rulare
                p = strtok(NULL, "  \n");
                proc.time = atoi(p);

                // prioritate
                p = strtok(NULL, "  \n");
                proc.prio = atoi(p);
                add(&ready, proc);

                // numele urmatorului proces (daca exista)
                p = strtok(NULL, "  \n");
                //printf("%s %d %d; ", nume, x, y);
            }
            tickPP(&running, &ready, cuanta, count);
            //printf("\n");
            break;

        case 'w': //wait

            // printf("wait\n");
            wait(&running, &waiting);
            tickPP(&running, &ready, cuanta, count);
            break;

        case 'e': //event
            // nume
            p = strtok(NULL, " \n");
            strcpy(proc.name, p);

            //printf("event: %s\n", nume);
            event(&waiting, &ready, proc.name);
            tickPP(&running, &ready, cuanta, count);
            break;

        case 's': //show
            //printf("show\n");
            show(running, fo);
            break;

        default:
            break;
        }

        printf("count: %d\n", *count);
        if (running != NULL)
            printf("running: %s %d %d\n", running->proc.name, running->proc.time, running->proc.prio);
        else
            printf("running: NULL\n");

        printf("ready: ");
        printList(ready);
        printf("waiting: ");
        printListDublu(waiting);
        printf("-------------------------------------------\n");
    }

    free(proc.name);
    free(count);

    node *aux = ready;
    while (aux != NULL)
    {
        ready = ready->next;
        free(aux->proc.name);
        free(aux);
        aux = ready;
    }

    dublu *auxD = waiting;
    while (auxD != NULL)
    {
        waiting = waiting->next;
        free(auxD->proc.name);
        free(auxD);
        auxD = waiting;
    }

    if (running)
    {
        free(running->proc.name);
        free(running);
    }
}

int main(int argc, char *argv[])
{
    FILE *f = fopen(argv[1], "r");
    FILE *fo = fopen(argv[2], "w");
    int tipAlgo, cuanta, nrNivPrior;
    readFirstLine(f, &tipAlgo, &cuanta, &nrNivPrior);

    switch (tipAlgo)
    {
    case 1: // FCFS
        FCFS(f, fo);
        break;

    case 2: // SJF
        SJF(f, fo);
        break;

    case 3: // Round Robin
        RR(f, fo, cuanta);
        break;

    case 4:                            // Planificare cu priorități
                                       // FCFS(f,fo);
        PP(f, fo, cuanta, nrNivPrior); //PlanificarePrioritati(f, cuanta, nrNivPrior);
        break;

    case 5: // Planificare cu priorități - bonus
        //PlanificarePrioritatiBonus(f, cuanta, nrNivPrior);
        break;
    }

    fclose(f);
    fclose(fo);
}
