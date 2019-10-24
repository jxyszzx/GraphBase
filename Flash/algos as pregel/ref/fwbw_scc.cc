#include "fwbw_scc.h"

int32_t color_scc(gm_graph& G, int32_t* G_color, 
    int32_t* G_id, int32_t* G_scc, 
    int32_t size)
{
    //Initializations
    gm_rt_initialize();
    G.freeze();
    G.prepare_edge_source();

    int32_t i = 0 ;
    int32_t nnum = 0 ;
    int32_t ret = 0 ;
    int32_t change = 0 ;
    int32_t x = 0 ;

    i = 0 ;
    nnum = size ;
    ret = 0 ;
    change = 1 ;
    x = 0 ;
    while (nnum > 0)
    {
        x = 0 ;
        nnum = 0 ;
        printf("round %d\n", i);
        i = i + 1 ;

        #pragma omp parallel for
        for (node_t n = 0; n < G.num_nodes(); n ++) 
        {
            if (G_scc[n] ==  -1)
            {
                G_color[n] = G_id[n] ;
            }
        }
        while (change > 0)
        {
            change = 0 ;
            #pragma omp parallel
            {
                int32_t change_prv = 0 ;

                change_prv = 0 ;

                #pragma omp for nowait
                for (edge_t e = 0; e < G.num_edges(); e ++) 
                {
                    node_t from;
                    node_t to;

                    from = G.node_idx_src[e] ;
                    to = G.node_idx[e] ;
                    if ((G_scc[from] ==  -1) && (G_scc[to] ==  -1) && (G_color[from] > G_color[to]))
                    {
                        G_color[to] = G_color[from] ;
                        change_prv = change_prv + 1 ;
                    }
                }
                ATOMIC_ADD<int32_t>(&change, change_prv);
            }
        }
        change = 1 ;
        printf("step 1 over \n");
        #pragma omp parallel
        {
            int32_t x_prv = 0 ;

            x_prv = 0 ;

            #pragma omp for nowait
            for (node_t n0 = 0; n0 < G.num_nodes(); n0 ++) 
            {
                if ((G_scc[n0] ==  -1) && (G_color[n0] == G_id[n0]))
                {
                    G_scc[n0] = G_id[n0] ;
                }
                if (G_scc[n0] ==  -1)
                {
                    x_prv = x_prv + 1 ;
                }
            }
            ATOMIC_ADD<int32_t>(&x, x_prv);
        }
        printf("still have %d \n",x);
        while (change > 0)
        {
            change = 0 ;
            #pragma omp parallel
            {
                int32_t change_prv7 = 0 ;

                change_prv7 = 0 ;

                #pragma omp for nowait
                for (edge_t e2 = 0; e2 < G.num_edges(); e2 ++) 
                {
                    node_t from3;
                    node_t to4;

                    from3 = G.node_idx_src[e2] ;
                    to4 = G.node_idx[e2] ;
                    if ((G_scc[to4] !=  -1) && (G_color[from3] == G_scc[to4]) && (G_scc[from3] ==  -1))
                    {
                        G_scc[from3] = G_scc[to4] ;
                        change_prv7 = change_prv7 + 1 ;
                    }
                }
                ATOMIC_ADD<int32_t>(&change, change_prv7);
            }
        }
        change = 1 ;
        printf("step 2 over \n");
        #pragma omp parallel
        {
            int32_t nnum_prv = 0 ;

            nnum_prv = 0 ;

            #pragma omp for nowait
            for (node_t n5 = 0; n5 < G.num_nodes(); n5 ++) 
            {
                if (G_scc[n5] ==  -1)
                {
                    nnum_prv = nnum_prv + 1 ;
                }
            }
            ATOMIC_ADD<int32_t>(&nnum, nnum_prv);
        }
        printf("still have %d \n",nnum);
    }
    #pragma omp parallel
    {
        int32_t ret_prv = 0 ;

        ret_prv = 0 ;

        #pragma omp for nowait
        for (node_t n6 = 0; n6 < G.num_nodes(); n6 ++) 
        {
            if (G_scc[n6] == G_id[n6])
            {
                ret_prv = ret_prv + 1 ;
            }
        }
        ATOMIC_ADD<int32_t>(&ret, ret_prv);
    }
    return ret; 
}

