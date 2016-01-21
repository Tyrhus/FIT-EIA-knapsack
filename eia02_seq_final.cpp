#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <algorithm>
#include <string.h>
#include <iostream>

//masatmir 2016


#define MAXIMAL 70  // maximalni pocet predmetu, maximal size of input

using namespace std;
// vstupni data, input data
struct item{
int cost; // cena polozky, cost of item
int weight; // vaha polozky, weight of item
float benefit; // benefit
bool operator()(const item& lhs, const item& rhs) const { return lhs.benefit > rhs.benefit; }
} array_of_items[MAXIMAL];

int tmp_max, benefit_max, benefit_start, cut;
int no_items,capacity;

char max_state[MAXIMAL];
char null_array[MAXIMAL];

  /*

  tmp_max = maximalni hodnota ceny batohu
  no_items = pocet veci
  capacity = nosnost batohu
  max_state = obsah optimalniho batohu
  max_state[i] = 0  kdyz i-ta polozka neni v optimalnim batohu


  tmp_max = maximal value of knapsack's price
  no_items = number of items
  capacity = capacity of knapsack
  max_state = content of the optimal knapsack
  max_state[i] = 0  if i-th item is not in the optimal knapsack

  muzete pridat nejake globalni promenne
  it is possible to add some global variables
  */


int testing(char *state)
{
  int i;
  int sc=0;
  int sw=0;
  for(i=0;i<no_items;i++)
    if (state[i]) {

    sc+=array_of_items[i].cost;
    sw+=array_of_items[i].weight;
    }

  if (sw>capacity) return -1;
  else return sc;
}


// zacatek casti k modifikaci
// beginning of part for modification

// chybi: preproccesing, vetve&hranice, paralelizace, ulozeni obsahu optimalniho batohu do max_state
// missing: preproccesing, B&B, parallelization, storing of content of optimal knapsack to max_state
void display_max_state()
{
  cout << "MAX combination" << endl;
  for (size_t i = 0; i < MAXIMAL; i++) {
    cout << max_state[i] << ", ";
  }
  cout << endl;
}

void display_array()
{
  cout<< "ARRAY REORDERED" << endl;
  for (int i = 0; i < no_items; ++i)
   {

        cout << "i = " << i << " price = " << array_of_items[i].cost
                            << " weight = " << array_of_items[i].weight
                            << " benefit = " << array_of_items[i].benefit << endl;
   }
}

void display_knapsack()
{
  int sum = 0;
  cout << "KNAPSACK CONTENT" << endl;
  for (int i = 0; i < no_items; ++i)
   {
      if (max_state[i])
      {
        cout << "VALID i = " << i << " price = " << array_of_items[i].cost  << " weight = " << array_of_items[i].weight << endl;
        sum += array_of_items[i].cost;
      }else
        cout << "INVALID i = " << i << " price = " << array_of_items[i].cost  << " weight = " << array_of_items[i].weight << endl;
   }
  cout <<  "KNAPSACK SUM = " << sum << endl;
}

int capacity_calc(int i, int left)
{
  int ret = 0;

  item *it = array_of_items + i;
  while (left > 0)
  {
    if (i == no_items) return ret;
    left = left - it->weight;
    ret  = ret  + it->cost;
    i++;
    it++;
  }

  it--;

  ret += (it->cost * left + it->weight - 1) /it->weight;

  return ret;
}

bool rek(int i,int fr_cap,int price, char* previous_max_state, int inserted, int benefit)
{

  /* i=index aktualniho predmetu
     fr_cap = volna kapacita v batohu
     price = aktualni cena batohu

     i=index of current item
     fr_cap = free capacity in knapsack
     price = actual price of knapsack    */

    if (benefit < tmp_max || i == no_items || fr_cap < 0)
      return true;

    if (!inserted)
    {
      previous_max_state[i-1] = 1;

      if (price>tmp_max)
      {
        tmp_max=price;
        memcpy(max_state, previous_max_state, no_items*sizeof(char));
      }
    }


      //#pragma omp task firstprivate(i, price, fr_cap, previous_max_state) default(shared)
        rek(i+1,fr_cap-array_of_items[i].weight,price+array_of_items[i].cost,previous_max_state,0,benefit);
      //#pragma omp task firstprivate(i, price, fr_cap, previous_max_state) default(shared)
        int newBenefit = price+capacity_calc(i+1, fr_cap);
        rek(i+1,fr_cap,price,previous_max_state,1,newBenefit);


  //#pragma omp taskwait
  if (!inserted)
   previous_max_state[i-1] = inserted;

   return true;

}
// end of part for modification
// konec casti k modifikaci




int main(void) {
int i,j,ok;
double start,end;
int   d,sum;
int correct[MAXIMAL+2]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18616, 18616, 20937, 20884, 23060, 23047, 25284, 25327, 27561, 27579, 29815, 31637, 32187, 34021, 34515, 36479, 36847, 38832, 39074, 41128, 41370, 43276, 43710, 45660, 45975, 47925, 48413, 50297, 50731, 52724, 53265, 55108, 55489, 57211, 57628, 59432, 59924, 61693, 62159, 63708, 64382, 66045, 66709, 68432, 69063, 70883, 71900, 73332, 74193, 75638, 76606, 77903, 78979, 80097, 81222, 82482};

  //omp_set_num_threads(4);
  no_items=15;
  omp_set_dynamic(0);
  start=omp_get_wtime();
  do{

  no_items++;
  sum=0.0;
  for(j=0;j<no_items;j++)
  {
    i=j+no_items;
    array_of_items[j].cost=2019+37*(i%5)+23*(i%7)+5*(i%31)+(i%3);
    array_of_items[j].weight=1771+19*(i%13)+7*(i%17)+5*(i%19)+(i%3);
    array_of_items[j].benefit=(float)array_of_items[j].cost / (float)array_of_items[j].weight;
    sum+=array_of_items[j].weight;
  }
  capacity=sum/2;

  //sort will make the branch and bound work right
  sort(array_of_items, array_of_items+no_items, array_of_items[0]);
  /
  // reset computation
  tmp_max=-1;

  //initial benefit calculation
  int cap2 = capacity_calc(0, capacity);

  /* vylepsete vykonnost tohoto volani !
     improve performance of this call ! */
  // int * null_array = (int *)malloc(no_items*sizeof(int));
  // for (int q=0; q< no_items; q++) null_array[q] = 0;


  rek(0,capacity,0,null_array,1,cap2);


  //cout << "cuts: " << ++cut << endl;

  // cout << "CAPACITY BEGINNING: " << capacity << endl;
  // display_knapsack();
  /*
  vysledek toho volani:
  tmp_max = maximalni hodnota ceny batohu
  max_state = obsah optimalniho batohu
  max_state[i] = 0  kdyz i-ta polozka neni v optimalnim batohu

  Result of this call:
  tmp_max = maximal value of knapsack's price
  max_state = content of the optimal knapsack
  max_state[i] = 0  if i-th item is not in the optimal knapsack
  */
  d= correct[no_items];
  if (d!=tmp_max)
  {
    printf("ERROR1=%i ret=%i correct=%i\n",no_items,tmp_max,d);
    return -1;
  }

  d=testing(max_state);
  if (d!=tmp_max)
  {
    printf("ERROR2=%i cap=%i tmp=%i\n",no_items,d,tmp_max);
    return -1;
  }
  ok=1;
  if (no_items>=MAXIMAL) ok=0;
  end=omp_get_wtime();
  if ((end-start)>=35.0) ok=0;

  }while(ok);
  printf("end=%i\n",no_items);
  printf("time=%g\n",end-start);


  return 0;
}
