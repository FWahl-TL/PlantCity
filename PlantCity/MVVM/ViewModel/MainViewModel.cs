﻿using PlantCity.Core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PlantCity.MVVM.ViewModel;

namespace PlantCity.MVVM.ViewModel
{
    class MainViewModel : ObservableObject
    {
        public RelayCommand HomeViewCommand { get; set; }
        public RelayCommand SettingsViewCommand { get; set; }
        public RelayCommand AboutViewCommand { get; set; }
        public RelayCommand ConnectorViewCommand { get; set; }


        public HomeViewModel HomeVM { get; set; }
        public SettingsViewModel SettingsVM { get; set; }
        public AboutViewModel AboutVM { get; set; }
        public ConnectorViewModel ConnectorVM { get; set; }

        private object _currentView;

        public object CurrentView
        {
            get { return _currentView; }
            set {
                _currentView = value;
                OnPropertyChanged();
            }
        }

        public MainViewModel()
        {
            HomeVM = new HomeViewModel();
            SettingsVM = new SettingsViewModel();
            AboutVM = new AboutViewModel();
            ConnectorVM = new ConnectorViewModel();
            CurrentView = HomeVM;

            HomeViewCommand = new RelayCommand(o => {CurrentView = HomeVM;});
            SettingsViewCommand = new RelayCommand(o => { CurrentView = SettingsVM; });
            AboutViewCommand = new RelayCommand(o => { CurrentView = AboutVM; });
            ConnectorViewCommand = new RelayCommand(o => { CurrentView = ConnectorVM; });
        }

    }
}
