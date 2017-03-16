#ifndef ALI_O2_ANALYSIS_MANAGER_H
#define ALI_O2_ANALYSIS_MANAGER_H
#include "O2AnalysisTask.h"
#include "ecs/Handler.h"
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

class O2AnalysisManager {
  std::vector<ecs::Handler> mHandlers;
  std::vector<std::unique_ptr<O2AnalysisTask>> mTasks;

public:
  void addFile(const std::string &file) { mHandlers.push_back(file); }

  // Create and add a new task to this manager.
  template <typename T, typename std::enable_if<std::is_base_of<
                            O2AnalysisTask, T>::value>::type * = nullptr,
            typename... Args>
  T &createNewTask(Args... args) {
    return *(T *)(&(emplaceTask(new T(args...))));
  }

  O2AnalysisTask &emplaceTask(O2AnalysisTask *&&task) {
    mTasks.emplace_back(task);
    return *mTasks[mTasks.size() - 1];
  }

  void startAnalysis();
};

#endif