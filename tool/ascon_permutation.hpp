//-----------------------------------------------------------------------------

template <unsigned rounds>
AsconPermutation<rounds>::AsconPermutation() {
  for(int i = 0; i< 2*rounds +1; ++i){
      this->state_masks_[i].reset(new AsconState);
    }
  for (int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(new AsconSboxLayer);
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(new AsconLinearLayer);
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
  touchall();
}

template <unsigned rounds>
AsconPermutation<rounds>::AsconPermutation(const AsconPermutation& other) {
  for(int i = 0; i< 2*rounds +1; ++i){
    this->state_masks_[i].reset(other.state_masks_[i]->clone());
  }

  for (int i = 0; i < rounds; ++i) {
    this->sbox_layers_[i].reset(other.sbox_layers_[i]->clone());
    this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
    this->linear_layers_[i].reset(other.linear_layers_[i]->clone());
    this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
  }
//  state_masks_ = other.state_masks_;
  toupdate_linear = other.toupdate_linear;
  toupdate_nonlinear = other.toupdate_nonlinear;
//  this->sbox_layers_ = other.sbox_layers_;
//  this->linear_layers_ = other.linear_layers_;

}

template <unsigned rounds>
AsconPermutation<rounds>& AsconPermutation<rounds>::operator=(const AsconPermutation<rounds>& rhs){
  for(int i = 0; i< 2*rounds +1; ++i){
    this->state_masks_[i].reset(rhs.state_masks_[i]->clone());
  }
// state_masks_ = rhs.state_maskss_;
 toupdate_linear = rhs.toupdate_linear;
 toupdate_nonlinear = rhs.toupdate_nonlinear;
// this->sbox_layers_ = rhs.sbox_layers_;
// this->linear_layers_ = rhs.linear_layers_;
 for (int i = 0; i < rounds; ++i) {
   this->sbox_layers_[i].reset(rhs.sbox_layers_[i]->clone());
   this->sbox_layers_[i]->SetMasks(this->state_masks_[2*i].get(), this->state_masks_[2*i + 1].get());
   this->linear_layers_[i].reset(rhs.linear_layers_[i]->clone());
   this->linear_layers_[i]->SetMasks(this->state_masks_[2*i + 1].get(), this->state_masks_[2*i + 2].get());
 }
 return *this;
}

template <unsigned rounds>
AsconPermutation<rounds>* AsconPermutation<rounds>::clone() const{
  return new AsconPermutation(*this);
}

template <unsigned rounds>
bool AsconPermutation<rounds>::checkchar() {
  bool correct;
  std::cout << "Characteristic before propagation" << std::endl << *this;
  correct = this->update();
  std::cout << "Characteristic after propagation" << std::endl << *this;
  return correct;
}

template <unsigned rounds>
bool AsconPermutation<rounds>::guessbestsbox(SboxPos pos) {
  AsconState tempin, tempout;

//    tempin = *((AsconState*) sbox_layers_[pos.layer_].in);
//    tempout = *((AsconState*) sbox_layers_[pos.layer_].out);

  this->sbox_layers_[pos.layer_]->GuessBox(UpdatePos(0, 0, pos.pos_, 0),0);

//    if (tempin.diff(*((AsconState*) sbox_layers_[pos.layer_].in)).size() != 0
//        || tempout.diff(*((AsconState*) sbox_layers_[pos.layer_].out)).size() != 0)
      toupdate_linear = true;
    return update();
}

template<unsigned rounds>
bool AsconPermutation<rounds>::guessbestsbox(SboxPos pos,
                                             int num_alternatives) {
  bool update_works = false;
  AsconPermutation<rounds> temp = *this;

  for (int i = 0; i < num_alternatives; ++i) {
    int total_alternatives = this->sbox_layers_[pos.layer_]->GuessBox(
        UpdatePos(0, 0, pos.pos_, 0), i);
    num_alternatives =
        total_alternatives < num_alternatives ?
            total_alternatives : num_alternatives;
    toupdate_linear = true;
    update_works = update();
    if (update_works)
      return update_works;
    *this = temp;
  }
  return false;
}

template <unsigned rounds>
bool AsconPermutation<rounds>::update() {
  //TODO: Better update
  bool correct = true;
  AsconState tempin, tempout;
  while (toupdate_linear == true || toupdate_nonlinear == true) {
    if (toupdate_nonlinear == true) {
      toupdate_nonlinear = false;
      for (size_t layer = 0; layer < rounds; ++layer) {
        tempin = *((AsconState*) this->sbox_layers_[layer]->in);
        tempout = *((AsconState*) this->sbox_layers_[layer]->out);
        for (int i = 0; i < 64; ++i)
          correct &= this->sbox_layers_[layer]->Update(UpdatePos(0, 0, i, 1));
        if (tempin.diff(*((AsconState*) this->sbox_layers_[layer]->in)).size() != 0
            || tempout.diff(*((AsconState*) this->sbox_layers_[layer]->out)).size() != 0)
          toupdate_linear = true;
      }
    }
    if (toupdate_linear == true) {
      toupdate_linear = false;
      for (size_t layer = 0; layer < rounds; ++layer) {
        tempin = *((AsconState*) this->linear_layers_[layer]->in);
        tempout = *((AsconState*) this->linear_layers_[layer]->out);
        for (int i = 0; i < 5; ++i)
          correct &= this->linear_layers_[layer]->Update(UpdatePos(0, i, 0, 1));
        if (tempin.diff(*((AsconState*) this->linear_layers_[layer]->in)).size() != 0
            || tempout.diff(*((AsconState*) this->linear_layers_[layer]->out)).size() != 0)
          toupdate_nonlinear = true;
      }
    }
  }
  return correct;
}

template<unsigned rounds>
ProbabilityPair AsconPermutation<rounds>::GetProbability() {
  ProbabilityPair prob { 1, 0.0 };
  ProbabilityPair temp_prob;

  for (auto& layer : this->sbox_layers_) {
    temp_prob = layer->GetProbability();
    prob.sign *= temp_prob.sign;
    prob.bias += temp_prob.bias;
  }

  prob.bias += rounds - 1;

  return prob;
}

template<unsigned rounds>
void AsconPermutation<rounds>::PrintWithProbability() {
  ProbabilityPair prob { 1, 0.0 };
  ProbabilityPair temp_prob;
  int active_sboxes = 0;
  for (int i = 0; i <= 2 * rounds; ++i) {
    std::cout << "State Mask " << i + 1;
    if (i % 2 == 0 && i < 2 * rounds) {
      temp_prob = this->sbox_layers_[i / 2]->GetProbability();
      int active_sboxes_layer = 0;
      for (int j = 0; j < 64; ++j)
        active_sboxes_layer += (int) this->sbox_layers_[i / 2]->SboxActive(j);
      active_sboxes += active_sboxes_layer;
      prob.sign *= temp_prob.sign;
      prob.bias += temp_prob.bias;
      std::cout << " sign: " << (int) temp_prob.sign << " bias: "
                << temp_prob.bias << " active sboxes: " << active_sboxes_layer;
    }
    std::cout << std::endl;
    this->state_masks_[i]->print();
    std::cout << std::endl;
//    std::cout << std::endl << state_masks_[i] << std::endl;
  }
  prob.bias += rounds - 1;
  std::cout << "Total: sign: " << (int) prob.sign << " bias: " << prob.bias
      << " active sboxes: " << active_sboxes << std::endl << std::endl;

  std::cout << "----------------------------------------------------------------" << std::endl;
}

template <unsigned rounds>
void AsconPermutation<rounds>::touchall() {
//for(int i = 0; i< state_masks_.size(); ++i){
//  for(int j = 0; j<64; ++j)
//    queue_nonlinear_.add_item(UpdatePos(i, 0, j, 0));
//  for(int j = 0; j<5; ++j)
//      queue_linear_.add_item(UpdatePos(i, j, 0, 0));
//
//}
  toupdate_linear = true;
  toupdate_nonlinear = true;
}


template <unsigned rounds>
void AsconPermutation<rounds>::SboxStatus(std::vector<SboxPos>& active,
                                  std::vector<SboxPos>& inactive) {
Permutation<rounds>::SboxStatus(active, inactive);
}
template <unsigned rounds>
void AsconPermutation<rounds>::SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive){
  Permutation<rounds>::SboxStatus(active, inactive);
}


//template <unsigned rounds>
//void AsconPermutation<rounds>::SboxStatus(std::vector<SboxPos>& active,
//                                  std::vector<SboxPos>& inactive) {
//  active.clear();
//  inactive.clear();
//
//  for (size_t layer = 0; layer < rounds; ++layer)
//    for (int pos = 0; pos < 64; ++pos)
//      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
//        if (this->sbox_layers_[layer]->SboxActive(pos))
//          active.emplace_back(layer, pos);
//        else
//          inactive.emplace_back(layer, pos);
//      }
//}
//template <unsigned rounds>
//void AsconPermutation<rounds>::SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive){
//  active.clear();
//  inactive.clear();
//  active.resize(rounds);
//  inactive.resize(rounds);
//
//  for (size_t layer = 0; layer < this->sbox_layers_.size(); ++layer)
//    for (int pos = 0; pos < 64; ++pos)
//      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
//        if (this->sbox_layers_[layer]->SboxActive(pos))
//          active[layer].emplace_back(layer, pos);
//        else
//          inactive[layer].emplace_back(layer, pos);
//      }
//}

