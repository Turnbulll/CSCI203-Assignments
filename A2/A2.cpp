#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void print(double records[500][2]);

class Customer
{
private:
  double arrivalTime, serviceTime;
public:
  Customer(){this->arrivalTime = 0; this->serviceTime = 0;};
  Customer(double arrival, double service){this->arrivalTime = arrival; this->serviceTime = service;};
  double getArrival(){return this->arrivalTime;};
  double getServiceTime(){return this->serviceTime;};
  void decreaseServiceTime(double serviceTime){this->serviceTime-=serviceTime;};
  void display();
};

class Server
{
private:
  double idleTime = 0;
public:
  Server(){};
  void increaseIdleTime(double timeStep){idleTime += timeStep;};
  void display(){cout << "Idle Time : " << idleTime << endl;};
};

class ArrivalQueue
{
private:
  Customer customerQueue[500];
  int tail = 0, head = 0, maxLength = 0;
public:
  ArrivalQueue(){};
  int getMax(){return maxLength;};
  void enqueue(Customer customer);
  Customer dequeue();
  bool isEmpty();
  void display();
};

class ServiceQueue
{
private:
  int serverNum;
  int serviceCount=0;
  Customer serviceQueue[4];
  Server servers[4];
public:
  ServiceQueue(){};
  ServiceQueue(int serverNum){this->serverNum=serverNum;};
  bool isBusy(){return serviceCount==serverNum;};
  bool isEmpty(){return serviceCount<=0;};
  void enqueue(Customer c);
  Customer dequeue();
  void siftDown(int i, int end);
  void makeHeap();
  void swap(Customer &c1, Customer &c2);
  void swap(Server &s1, Server &s2);
  void display();
  void decreaseServiceTime(double serviceTime);
  double timeTillEOS(){return this->serviceQueue[0].getServiceTime();};
  void increaseIdleTimeForAllServers(double timeStep);
  void displayIdleServers();
};

void printDebug(double currentTime, ServiceQueue sq, ArrivalQueue q);
double calcTimeStep(double &currentTime, ServiceQueue sq);
double calcTimeStep(double &currentTime, Customer c);
void displayResults(int custCount, double currentTime, double averageResponseTime, double averageServiceTime, double estAvgQueueLength, int maxLengths, ServiceQueue &sq);
double calcAvg(double sum, int n);

int main () {
    string filename, line, word;
    double currentTime = 0, totalResponseTime = 0, totalServiceTime = 0;
    int serverNum, custCount = 0;
    ArrivalQueue q;
    

    cout << "Enter amount of servers: " << endl;
    cin >> serverNum;
    ServiceQueue sq(serverNum);
    cout << "Enter Filename: " << endl;
    cin >> filename;
    //ifstream myfile ("a2-sample.txt");
    ifstream myfile (filename);
    if (myfile.is_open())
    {
      while ( getline (myfile, line) )
      { 
        stringstream ss(line);
        getline(ss, word, ' ');
        if(word != "0" && !word.empty()){
          double arrTime = stof(word);
          getline(ss, word, ' ');
          double servTime = stof(word);
          Customer c(arrTime, servTime);
          totalServiceTime += c.getServiceTime();
          custCount++;
          //If arrival is the next event customer goes straight into the service queue or arrival queue
          if(c.getArrival() < currentTime + sq.timeTillEOS() || sq.isEmpty()){
            double timeStep = calcTimeStep(currentTime, c);
            sq.increaseIdleTimeForAllServers(timeStep);
            sq.decreaseServiceTime(timeStep);
            if(!sq.isBusy()){//if not busy go in service queue
              sq.enqueue(c);
            }
            else{//if service queue is busy go into the arrival queue
              q.enqueue(c);
            }
          }
          else{ // if the end of service (EOS) comes before a customer arrival and then deals with the next customer arrival
            while(c.getArrival() > currentTime + sq.timeTillEOS() && !sq.isEmpty()){
              double timeStep = calcTimeStep(currentTime, sq);
              sq.increaseIdleTimeForAllServers(timeStep);
              sq.decreaseServiceTime(timeStep);
              Customer finishedService = sq.dequeue();
              totalResponseTime += currentTime - finishedService.getArrival();
            }
            if(!q.isEmpty()){ //If the queue is not empty shuffle the customer from the arrival queue into the service queue
              Customer nextForService = q.dequeue();
              sq.enqueue(nextForService);
              double timeStep = calcTimeStep(currentTime, c);
              sq.increaseIdleTimeForAllServers(timeStep);
              q.enqueue(c);
              sq.decreaseServiceTime(timeStep);
            }
            else{
              double timeStep = calcTimeStep(currentTime, c);
              sq.increaseIdleTimeForAllServers(timeStep);
              sq.decreaseServiceTime(timeStep); 
              if(!sq.isBusy()){//if not busy go in service queue
                sq.enqueue(c);
              }
              else{//if service queue is busy go into the arrival queue
                q.enqueue(c);
              }
            }
          }
        }
      }
      myfile.close();
      while(!q.isEmpty()){ //Empty arrival queue when eof has been reached
        double timeStep = calcTimeStep(currentTime, sq);
        sq.increaseIdleTimeForAllServers(timeStep);
        sq.decreaseServiceTime(timeStep);
        Customer finishedService = sq.dequeue();
        totalResponseTime += currentTime - finishedService.getArrival();
        Customer c = q.dequeue(); 
        sq.enqueue(c);
      }
      while(!sq.isEmpty()){
        double timeStep = calcTimeStep(currentTime, sq);
        sq.increaseIdleTimeForAllServers(timeStep);
        sq.decreaseServiceTime(timeStep);
        Customer finishedService = sq.dequeue();
        totalResponseTime += currentTime - finishedService.getArrival();
      }
      double averageResponseTime = calcAvg(totalResponseTime, custCount);
      double averageServiceTime = calcAvg(totalServiceTime, custCount);
      double totalQueueTime = totalResponseTime - totalServiceTime;
      double estAvgQueueLength = totalQueueTime/currentTime;
      int maxLength = q.getMax();
      displayResults(custCount, currentTime, averageResponseTime, averageServiceTime, estAvgQueueLength, maxLength , sq);
    }
    else cout << "Unable to open file";
    return 0;
}

void displayResults(int custCount, double currentTime, double averageResponseTime, double averageServiceTime, double estAvgQueueLength, int maxLength, ServiceQueue &sq){
  cout << "Number of people served: " << custCount << endl;
  cout << "Time last service request is completed: " << currentTime << endl;
  cout << "Average total service time per customer: " << averageResponseTime <<  endl; //(this includes the time spent in a queue)
  cout << "Average total time in service per customer: " << averageServiceTime << endl; //(this excludes the time spent in a queue)
  cout << "Average length of the queue: " << estAvgQueueLength << endl;  //(this can be estimated as the ratio between total queuing time and the time last service request is completed)
  cout << "Maximum length: " << maxLength << endl; // i.e. number of customers, of the queue.
  cout << "Total idle time for each server: " << endl;
  sq.displayIdleServers();
}

//Calculate the time jump for the event
double calcTimeStep(double &currentTime, ServiceQueue sq){
  double prevTime = currentTime;
  currentTime += sq.timeTillEOS();
  double timeStep = currentTime - prevTime;
  return timeStep;
}

double calcTimeStep(double &currentTime, Customer c){
  double prevTime = currentTime;
  currentTime = c.getArrival();
  double timeStep = currentTime - prevTime;
  return timeStep;
}

//Calculates an average
double calcAvg(double sum, int n){
  return sum/n; 
}

//Debug code
void printDebug(double currentTime, ServiceQueue sq, ArrivalQueue q){
  cout << "Current Time is: " <<  currentTime << endl;
  cout << endl;

  cout << "Service Queue" << endl;
  sq.display();
  cout << endl;

  cout << "Arrival Queue" << endl;
  q.display();
  cout << endl;
}

void Customer::display(){
  cout << "Arrival Time: " << this->arrivalTime << ", Service Time: " << this->serviceTime << endl;
}

void ArrivalQueue::enqueue(Customer customer){
  this->customerQueue[this->tail%500] = customer; 
  this->tail++;
  if(this->tail - this->head > maxLength){
    this->maxLength = this->tail - this->head;
  }
}

Customer ArrivalQueue::dequeue(){
  Customer customer = this->customerQueue[head%500];
  this->head++;
  return customer;
}

bool ArrivalQueue::isEmpty(){
  if(this->tail - this->head <= 0){
    return true;
  }
  return false;
}

void ArrivalQueue::display(){
  for(int i = head; i < tail; i++){
    this->customerQueue[i%500].display();
  }
}

void ServiceQueue::enqueue(Customer c){
  serviceQueue[serviceCount] = c;
  serviceCount++;
  makeHeap();
}

Customer ServiceQueue::dequeue(){
  Customer c = serviceQueue[0]; 
  swap(serviceQueue[0], serviceQueue[serviceCount-1]);
  serviceCount--;
  makeHeap();
  return c;
}

void ServiceQueue::siftDown(int i, int end){
  while(i * 2 + 1 < end){
    int child = 2*i +1;
    if((child + 1 < end) && serviceQueue[child].getServiceTime() > serviceQueue[child + 1].getServiceTime()){
      child++;
    }
    if(serviceQueue[i].getServiceTime() > serviceQueue[child].getServiceTime()){
      swap(serviceQueue[i], serviceQueue[child]);
      swap(servers[i], servers[child]);
      i = child;
    }
    else{
      return;
    }
  }
}

void ServiceQueue::makeHeap(){
  for(int i = (serviceCount-1)/2; i>=0 ; i--){
    //cout<< "i: " << i << endl;
    siftDown(i, serviceCount-1);
  }
}

void ServiceQueue::swap(Customer &c1, Customer&c2){
  Customer temp = c1;
  c1 = c2;
  c2 = temp;
}

void ServiceQueue::swap(Server &s1, Server&s2){
  Server temp = s1;
  s1 = s2;
  s2 = temp;
}

void ServiceQueue::display(){
  for(int i = 0; i < serviceCount; i++){
    serviceQueue[i].display();
  }
}

void ServiceQueue::decreaseServiceTime(double serviceTime){
  for(int i = 0; i < serviceCount; i++){
    serviceQueue[i].decreaseServiceTime(serviceTime);
  }
}

void ServiceQueue::increaseIdleTimeForAllServers(double timeStep){
  for(int i = serviceCount ; i < serverNum; i++){
    servers[i].increaseIdleTime(timeStep);
  }
}

void ServiceQueue::displayIdleServers(){
  for(int i = 0; i < serverNum; i++){
    cout << "Server: " << i+1 << ". ";
    servers[i].display();
  }
}




