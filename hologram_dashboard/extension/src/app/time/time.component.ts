import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-time',
  templateUrl: './time.component.html',
  styleUrls: ['./time.component.css']
})
export class TimeComponent implements OnInit {
  time: String;

  constructor() { }

  ngOnInit(): void {
    this.time = "TODO";
  }

}
