#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <vector>

struct DetourObject{
    DetourObject(): originalPathIndex(-1) {};
    double pathMetric;
    double forwardPathMetric;
    int detourStage;
    int startingState;
    int originalPathIndex;         //path that is being detoured from, defaults to -1 to indicate no detours
    
    bool operator<(const DetourObject& obj){
        return pathMetric < obj.pathMetric;
    }
    bool operator>(const DetourObject& obj){
        return pathMetric > obj.pathMetric;
    }
};

struct DetourObject_fixedp{
    DetourObject_fixedp(): originalPathIndex(-1) {};
    int32_t pathMetric;
    int32_t forwardPathMetric;
    int detourStage;
    int startingState;
    int originalPathIndex;         //path that is being detoured from, defaults to -1 to indicate no detours
    
    bool operator<(const DetourObject_fixedp& obj){
        return pathMetric < obj.pathMetric;
    }
    bool operator>(const DetourObject_fixedp& obj){
        return pathMetric > obj.pathMetric;
    }
};

class MinHeap{
public:
    MinHeap();
    void insert(DetourObject);
    DetourObject pop();
    DetourObject top();
    int size();
private:
    std::vector<DetourObject> detourList;
    void reHeap(int index);
    int parentIndex(int index);
    int rightChildIndex(int index);
    int leftChildIndex(int index);
};

class MinHeap_fixedp{
    public:
        MinHeap_fixedp();
        void insert(DetourObject_fixedp);
        DetourObject_fixedp pop();
        DetourObject_fixedp top();
        int size();
    private:
        std::vector<DetourObject_fixedp> detourList;
        void reHeap(int index);
        int parentIndex(int index);
        int rightChildIndex(int index);
        int leftChildIndex(int index);
    };


#endif