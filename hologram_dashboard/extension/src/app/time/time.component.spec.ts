import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { TimeComponent } from './time.component';

describe('TimeComponent', () => {
  let component: TimeComponent;
  let fixture: ComponentFixture<TimeComponent>;
  let time: HTMLElement

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ TimeComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(TimeComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
    time = fixture.nativeElement.querySelector('p');
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });

  it('should have'), () => {
    expect(time.textContent).toEqual('Last Refreshed: TODO')
  }
});
